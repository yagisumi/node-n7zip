#include "reader.h"
#include "create_reader_args.h"
#include "create_reader_worker.h"
#include "close_worker.h"
#include "get_property_info_worker.h"
#include "get_archive_properties_worker.h"

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
      InstanceMethod("getNumberOfItems", &Reader::GetNumberOfItems),
      InstanceMethod("getNumberOfArchiveProperties", &Reader::GetNumberOfArchiveProperties),
      InstanceMethod("getNumberOfProperties", &Reader::GetNumberOfProperties),
      InstanceMethod("isClosed", &Reader::IsClosed),
      InstanceMethod("close", &Reader::Close),
      InstanceMethod("getPropertyInfo", &Reader::GetPropertyInfo),
      InstanceMethod("getArchiveProperties", &Reader::GetArchiveProperties),
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  return exports;
}

Napi::Object
Reader::New(Napi::Env,
            int fmt_index,
            CMyComPtr<IInArchive>& archive,
            CMyComPtr<IArchiveOpenCallback>& open_callback)
{
  auto obj = constructor.New({});
  auto self = Napi::ObjectWrap<Reader>::Unwrap(obj);

  self->m_fmt_index = fmt_index;
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
Reader::GetNumberOfItems(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Reader::GetNumberOfItems]");
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
  }

  new GetArchivePropertiesWorker(env, callback, this, std::move(prop_ids));

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
Reader::get_entries(UInt32 start, UInt32 end, std::vector<PROPID>& prop_ids)
{
  std::vector<Entry> entries;

  auto locked = lock();
  if (m_closed) {
    return entries;
  }

  for (UInt32 i = start; i < end; i++) {
    std::vector<EntryProperty> props(prop_ids.size());
    for (size_t pidx = 0; pidx < prop_ids.size(); pidx++) {
      props[pidx].prop_id = prop_ids[pidx];
      m_archive->GetProperty(i, prop_ids[pidx], &props[pidx].prop);
    }

    entries.emplace_back(i, std::move(props));
  }

  return entries;
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
