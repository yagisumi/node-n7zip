#include "reader.h"
#include "create_reader_args.h"
#include "create_reader_worker.h"
#include "close_worker.h"
#include "get_property_info_worker.h"
#include "get_archive_properties_worker.h"
#include "get_entries_worker.h"
#include "../canceler.h"
#include "extract_worker.h"

namespace n7zip {

static std::unique_ptr<std::vector<PROPID>>
get_prop_ids(Napi::Array ary)
{
  auto prop_ids = std::make_unique<std::vector<PROPID>>();

  for (uint32_t i = 0; i < ary.Length(); i++) {
    auto v = ary.Get(i);
    if (v.IsNumber()) {
      auto num = v.ToNumber().Int32Value();
      if (num >= 0) {
        prop_ids->push_back(num);
      }
    }
  }

  return prop_ids;
}

Napi::FunctionReference Reader::constructor;

Napi::Object
Reader::Init(Napi::Env env, Napi::Object exports)
{
  auto func = DefineClass( //
    env,
    "Reader",
    {
      InstanceAccessor("formatIndex", &Reader::GetFormatIndex, nullptr),
      InstanceAccessor("formatName", &Reader::GetFormatName, nullptr),
      InstanceAccessor("numberOfEntries", &Reader::GetNumberOfEntries, nullptr),
      InstanceAccessor("numberOfArchiveProperties", &Reader::GetNumberOfArchiveProperties, nullptr),
      InstanceAccessor("numberOfProperties", &Reader::GetNumberOfProperties, nullptr),

      InstanceMethod("isClosed", &Reader::IsClosed),
      InstanceMethod("close", &Reader::Close),
      InstanceMethod("getPropertyInfo", &Reader::GetPropertyInfo),
      InstanceMethod("getArchiveProperties", &Reader::GetArchiveProperties),
      InstanceMethod("getEntries", &Reader::GetEntries),
      InstanceMethod("extract", &Reader::Extract),
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  return exports;
}

Napi::Object
Reader::New(Napi::Env,
            int fmt_index,
            std::string&& fmt_name,
            CMyComPtr<IInArchive>& archive,
            CMyComPtr<IArchiveOpenCallback>& open_callback)
{
  auto obj = constructor.New({});
  auto self = Napi::ObjectWrap<Reader>::Unwrap(obj);

  self->m_fmt_index = fmt_index;
  self->m_fmt_name = std::move(fmt_name);
  self->m_archive = archive;
  self->m_open_callback = open_callback;

  self->m_archive->GetNumberOfItems(&self->m_num_of_items);
  self->m_archive->GetNumberOfArchiveProperties(&self->m_num_of_arc_props);
  self->m_archive->GetNumberOfProperties(&self->m_num_of_props);
  self->m_closed.store(false);

  return obj;
}

Reader::Reader(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<Reader>(info)
{
  TRACE_THIS("+ Reader");
}

Reader::~Reader()
{
  TRACE_THIS("- Reader");
}

Napi::Value
Reader::GetFormatIndex(const Napi::CallbackInfo& info)
{
  return Napi::Number::New(info.Env(), m_fmt_index);
}

Napi::Value
Reader::GetFormatName(const Napi::CallbackInfo& info)
{
  return Napi::String::New(info.Env(), m_fmt_name.c_str());
}

Napi::Value
Reader::GetNumberOfEntries(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetNumberOfEntries]");
  auto env = info.Env();
  if (m_archive) {
    return Napi::Number::New(env, m_num_of_items);
  } else { // unexpected
    return Napi::Number::New(env, 0);
  }
}

Napi::Value
Reader::GetNumberOfArchiveProperties(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetNumberOfArchiveProperties]");
  auto env = info.Env();
  if (m_archive) {
    return Napi::Number::New(env, m_num_of_arc_props);
  } else { // unexpected
    return Napi::Number::New(env, 0);
  }
}

Napi::Value
Reader::GetNumberOfProperties(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetNumberOfProperties]");
  auto env = info.Env();
  if (m_archive) {
    return Napi::Number::New(env, m_num_of_props);
  } else { // unexpected
    return Napi::Number::New(env, 0);
  }
}

Napi::Value
Reader::IsClosed(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::IsClosed]");
  auto env = info.Env();

  if (m_archive) {
    return Napi::Boolean::New(env, m_closed.load());
  } else {
    return Napi::Boolean::New(env, true);
  }
}

Napi::Value
Reader::Close(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::close]");
  auto env = info.Env();

  if (!m_archive) {
    return ERR(env, "Uninitialized Reader");
  }

  if (info.Length() == 0 || !(info[0].IsFunction())) {
    return ERR(env, "No callback function was given");
  }

  auto callback = info[0].As<Napi::Function>();

  if (m_closed.load()) {
    try {
      callback.Call({ OK(env) });
    } catch (...) {
    }
    return OK(env);
  }

  new CloseWorker(env, callback, this);

  return OK(env);
}

Napi::Value
Reader::GetPropertyInfo(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetPropertyInfo]");
  auto env = info.Env();

  if (!m_archive) {
    return ERR(env, "Uninitialized Reader");
  }

  if (m_closed.load()) {
    return ERR(env, "Reader is already closed");
  }

  if (info.Length() == 0 || !(info[0].IsFunction())) {
    return ERR(env, "No callback function was given");
  }

  auto callback = info[0].As<Napi::Function>();

  new GetPropertyInfoWorker(env, callback, this);

  return OK(env);
}

Napi::Value
Reader::GetArchiveProperties(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetArchiveProperties]");
  auto env = info.Env();

  if (!m_archive) {
    return ERR(env, "Uninitialized Reader");
  }

  if (m_closed.load()) {
    return ERR(env, "Reader is already closed");
  }

  if (info.Length() < 2) {
    return ERR(env, "The arguments must be Object and callback function");
  }

  if (info[0].IsArray() || !info[0].IsObject()) {
    return ERR(env, "The first arguments must be Object");
  }

  if (!info[1].IsFunction()) {
    return ERR(env, "The second arguments must be callback function");
  }

  std::unique_ptr<std::vector<PROPID>> prop_ids;
  auto opts = info[0].ToObject();
  auto callback = info[1].As<Napi::Function>();

  auto prop_ary = opts.Get("propIDs");
  if (prop_ary.IsArray()) {
    prop_ids = get_prop_ids(prop_ary.As<Napi::Array>());
    if (prop_ids->empty()) {
      return ERR(env, "No valid value for 'propIDs'");
    }
  }

  new GetArchivePropertiesWorker(env, callback, this, std::move(prop_ids));

  return OK(env);
}

Napi::Value
Reader::GetEntries(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetEntries]");
  auto env = info.Env();

  if (!m_archive) {
    return ERR(env, "Uninitialized Reader");
  }

  if (m_closed.load()) {
    return ERR(env, "Reader is already closed");
  }

  if (info.Length() < 2) {
    return ERR(env, "The arguments must be Object and callback function");
  }

  if (info[0].IsArray() || !info[0].IsObject()) {
    return ERR(env, "The first arguments must be Object");
  }

  if (!info[1].IsFunction()) {
    return ERR(env, "The second arguments must be callback function");
  }

  UInt32 limit = 100;
  UInt32 start = 0;
  UInt32 end = m_num_of_items;
  std::unique_ptr<std::vector<PROPID>> prop_ids;

  auto opts = info[0].ToObject();
  auto callback = info[1].As<Napi::Function>();

  auto prop_ary = opts.Get("propIDs");
  if (prop_ary.IsArray()) {
    prop_ids = get_prop_ids(prop_ary.As<Napi::Array>());
    if (prop_ids->empty()) {
      return ERR(env, "No valid value for 'propIDs'");
    }
  }

  auto limit_v = opts.Get("limit");
  if (limit_v.IsNumber()) {
    auto limit_num = limit_v.ToNumber().Int32Value();
    if (limit_num > 0) {
      limit = limit_num;
    }
  }

  auto start_v = opts.Get("start");
  if (start_v.IsNumber()) {
    auto start_num = start_v.ToNumber().Int32Value();
    if (0 < start_num && (UInt32)start_num < m_num_of_items) {
      start = start_num;
    } else {
      return ERR(env,
                 "'start' value is out of range (start: %d, min: 0, max: %d)",
                 start_num,
                 m_num_of_items - 1);
    }
  }

  auto end_v = opts.Get("end");
  if (end_v.IsNumber()) {
    auto end_num = end_v.ToNumber().Int32Value();
    if (0 < end_num && (UInt32)end_num <= m_num_of_items) {
      end = end_num;
    } else {
      return ERR(
        env, "'end' value is out of range (end: %d, min: 1, max: %u)", end_num, m_num_of_items);
    }
  }

  if (start >= end) {
    return ERR(env, "'start' must be less than 'end' (start: %u, end: %u)", start, end);
  }

  Canceler* canceler = nullptr;
  auto canceler_v = opts.Get("canceler");
  if (canceler_v.IsObject()) {
    auto canceler_obj = canceler_v.ToObject();
    if (canceler_obj.InstanceOf(Canceler::constructor.Value())) {
      canceler = Napi::ObjectWrap<Canceler>::Unwrap(canceler_obj);
    }
  }

  new GetEntriesWorker(
    env, callback, this, GetEntriesArgs(limit, start, end, std::move(prop_ids), canceler));

  return OK(env);
}

Napi::Value
Reader::Extract(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::Extract]");
  auto env = info.Env();

  if (!m_archive) {
    return ERR(env, "Uninitialized Reader");
  }

  if (m_closed.load()) {
    return ERR(env, "Reader is already closed");
  }

  if (info.Length() < 2) {
    return ERR(env, "The arguments must be Object and callback function");
  }

  if (info[0].IsArray() || !info[0].IsObject()) {
    return ERR(env, "The first arguments must be Object");
  }

  if (!info[1].IsFunction()) {
    return ERR(env, "The second arguments must be callback function");
  }

  auto opts = info[0].ToObject();
  auto callback = info[1].As<Napi::Function>();

  if (!opts.Has("index")) {
    return ERR(env, "'index' value is not specified");
  }

  auto index_v = opts.Get("index");
  if (!index_v.IsNumber()) {
    return ERR(env, "'index' must be Number");
  }

  auto index_tmp = index_v.ToNumber().Int64Value();
  if (index_tmp < 0) {
    return ERR(env,
               "'index' value is out of range (index: %lld, min: 0, max: %u)",
               index_tmp,
               m_num_of_items - 1);
  }

  auto index = index_v.ToNumber().Uint32Value();
  if (index >= m_num_of_items) {
    return ERR(
      env, "'index' value is out of range (index: %u, min: 0, max: %u)", index, m_num_of_items - 1);
  }

  auto limit_v = opts.Get("limit");
  UInt32 limit = 1048576;
  if (!limit_v.IsUndefined()) {
    if (!limit_v.IsNumber()) {
      return ERR(env, "'limit' must be Number");
    }

    limit = std::max(1048576, limit_v.ToNumber().Int32Value());
  }

  bool test_mode = false;
  auto test_mode_v = opts.Get("testMode");
  if (test_mode_v.IsBoolean()) {
    test_mode = test_mode_v.ToBoolean();
  }

  new ExtractWorker(env, callback, this, index, limit, test_mode);

  return OK(env);
}

std::unique_lock<std::recursive_mutex>
Reader::lock()
{
  return std::unique_lock<std::recursive_mutex>(m_mutex);
}

HRESULT
Reader::close()
{
  if (!m_closed) {
    auto locked = lock();
    auto r = m_archive->Close();
    if (r == S_OK) {
      m_closed.store(true);
    }
    return r;
  } else {
    return S_OK;
  }
}

std::unique_ptr<ReaderPropertyInfo>
Reader::get_property_info()
{
  auto locked = lock();
  if (m_closed) {
    return std::unique_ptr<ReaderPropertyInfo>();
  }

  auto info = std::make_unique<ReaderPropertyInfo>();

  info->archive.resize(m_num_of_arc_props);
  info->entry.resize(m_num_of_props);

  for (UInt32 i = 0; i < m_num_of_arc_props; i++) {
    m_archive->GetArchivePropertyInfo(
      i, &info->archive[i].name, &info->archive[i].pid, &info->archive[i].type);
  }

  for (UInt32 i = 0; i < m_num_of_props; i++) {
    m_archive->GetPropertyInfo(i, &info->entry[i].name, &info->entry[i].pid, &info->entry[i].type);
  }

  return info;
}

std::vector<EntryProperty>
Reader::get_archive_properties(std::unique_ptr<std::vector<PROPID>>& prop_ids)
{
  std::vector<EntryProperty> props;

  auto locked = lock();
  if (m_closed) {
    return props;
  }

  if (!prop_ids) {
    prop_ids = std::make_unique<std::vector<PROPID>>();
    prop_ids->resize(m_num_of_arc_props);
    for (UInt32 i = 0; i < m_num_of_arc_props; i++) {
      VARTYPE ver_type;
      CMyComBSTR2 name;
      m_archive->GetArchivePropertyInfo(i, &name, &(*prop_ids)[i], &ver_type);
    }
  }

  props.resize(prop_ids->size());
  for (size_t i = 0; i < prop_ids->size(); i++) {
    auto& prop = props[i];
    prop.prop_id = (*prop_ids)[i];
    m_archive->GetArchiveProperty(prop.prop_id, &prop.prop);
  }

  return props;
}

std::vector<Entry>
Reader::get_entries(UInt32 start, UInt32 end, std::unique_ptr<std::vector<PROPID>>& prop_ids)
{
  TRACE_THIS("[Reader::get_entries] start: %u, end: %u, prop_ids: %d", start, end, !!prop_ids);
  std::vector<Entry> entries;

  auto locked = lock();
  if (m_closed) {
    return entries;
  }

  if (!prop_ids) {
    prop_ids = std::make_unique<std::vector<PROPID>>();
    prop_ids->resize(m_num_of_props);

    for (UInt32 i = 0; i < m_num_of_props; i++) {
      VARTYPE ver_type;
      CMyComBSTR2 name;
      m_archive->GetPropertyInfo(i, &name, &(*prop_ids)[i], &ver_type);
    }
  }

  for (UInt32 i = start; i < end; i++) {
    std::vector<EntryProperty> props(prop_ids->size());
    for (size_t pidx = 0; pidx < prop_ids->size(); pidx++) {
      props[pidx].prop_id = (*prop_ids)[pidx];
      m_archive->GetProperty(i, (*prop_ids)[pidx], &props[pidx].prop);
    }

    entries.emplace_back(i, std::move(props));
  }

  return entries;
}

HRESULT
Reader::extract(const UInt32* indices,
                UInt32 numItems,
                Int32 testMode,
                IArchiveExtractCallback* extractCallback)
{
  auto locked = lock();
  if (m_closed) {
    return S_FALSE;
  }
  return m_archive->Extract(indices, numItems, testMode, extractCallback);
}

HRESULT
Reader::is_dir(const UInt32 index, bool& result)
{
  auto locked = lock();
  result = false;

  if (m_closed) {
    return E_FAIL;
  }

  NWindows::NCOM::CPropVariant prop;
  auto r = m_archive->GetProperty(index, kpidIsDir, &prop);
  if (r != S_OK) {
    return r;
  } else if (prop.vt == VT_BOOL) {
    result = prop.boolVal != VARIANT_FALSE;
  } else if (prop.vt == VT_EMPTY) {
    result = false;
  } else {
    return E_FAIL;
  }
  return S_OK;
}

bool
Reader::is_link(const UInt32 index)
{
  auto locked = lock();
  if (m_closed) {
    return false;
  }

  {
    NWindows::NCOM::CPropVariant prop;
    auto r = m_archive->GetProperty(index, kpidAttrib, &prop);
    if (r == S_OK && prop.vt == VT_UI4) {
      if (prop.vt == VT_UI4 && (prop.ulVal & FILE_ATTRIBUTE_REPARSE_POINT)) {
        return true;
      }
    }
  }

  {
    NWindows::NCOM::CPropVariant prop;
    auto r = m_archive->GetProperty(index, kpidPosixAttrib, &prop);
    if (r == S_OK && prop.vt == VT_UI4) {
      if (prop.vt == VT_UI4 && (MY_LIN_S_ISLNK(prop.ulVal))) {
        return true;
      }
    }
  }

  return false;
}

static Napi::Object
createReader(const Napi::CallbackInfo& info)
{
  TRACE("createReader");
  auto env = info.Env();

  if ((info.Length() == 0) || (info[0].IsArray() || !info[0].IsObject())) {
    return TYPE_ERR(env, "The first argument must be Object");
  }

  if ((info.Length() < 2) || !info[1].IsFunction()) {
    return TYPE_ERR(env, "The second argument must be callback function");
  }

  auto callback = info[1].As<Napi::Function>();

  auto arg = info[0].ToObject();
  auto result = buildCreateReaderArg(arg);
  if (result.err()) {
    return result.err()->ERR(env);
  } else if (!result.ok()) {
    return ERR(env, "Unexpected error");
  }

  new CreateReaderWorker(env, callback, result.move_ok());

  return OK(env);
}

Napi::Object
InitReader(Napi::Env env, Napi::Object exports)
{
  Reader::Init(env, exports);
  exports.Set("createReader", Napi::Function::New(env, createReader));

  return exports;
}

} // namespace n7zip
