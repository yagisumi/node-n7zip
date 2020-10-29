#include "reader.h"

namespace n7zip {

Reader::Reader(int fmt_index,
               CMyComPtr<IInArchive>& archive,
               CMyComPtr<IArchiveOpenCallback>& open_callback)
  : m_fmt_index(fmt_index)
  , m_archive(archive)
  , m_open_callback(open_callback)
{
  TRACE("+ Reader %p", this);
  m_closed.store(false);
}

Reader::~Reader()
{
  TRACE("- Reader %p", this);
}

bool
Reader::close()
{
  if (!m_closed) {
    auto locked = lock();
    auto r = m_archive->Close();
    if (r == S_OK) {
      m_closed.store(true);
    } else {
      return false;
    }
  }

  return true;
}

Napi::FunctionReference ReaderWrap::constructor;

Napi::Object
ReaderWrap::Init(Napi::Env env, Napi::Object exports)
{
  auto func = DefineClass( //
    env,
    "Reader",
    {
      InstanceMethod("isClosed", &ReaderWrap::isClosed),
      InstanceMethod("close", &ReaderWrap::close),
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  return exports;
}

ReaderWrap::ReaderWrap(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<ReaderWrap>(info)
{
  TRACE("- ReaderWrap %p", this);
}

ReaderWrap::~ReaderWrap()
{
  TRACE("- ReaderWrap %p", this);
}

Napi::Value
ReaderWrap::isClosed(const Napi::CallbackInfo& info)
{
  auto env = info.Env();

  if (m_reader) {
    return Napi::Boolean::New(env, m_reader->m_closed.load());
  } else {
    return Napi::Boolean::New(env, true);
  }
}

Napi::Value
ReaderWrap::close(const Napi::CallbackInfo& info)
{
  return info.Env().Undefined();
}

static std::vector<int32_t>
getFormatIndices(Napi::Array fmt_index_ary)
{
  std::vector<int32_t> fmt_indices;
  auto fmt_len = g_library_info->get_formats_length();

  for (uint32_t i = 0; i < fmt_index_ary.Length(); i++) {
    auto elem = fmt_index_ary.Get(i);
    if (elem.IsNumber()) {
      auto num = elem.ToNumber();
      auto n = num.Int32Value();
      if (n >= 0 && n < fmt_len) {
        fmt_indices.push_back(n);
      }
    }
  }

  return fmt_indices;
}

static Napi::Object
createReader(const Napi::CallbackInfo& info)
{
  TRACE("createReader");
  auto lock = g_library_info->get_shared_lock();
  auto env = info.Env();

  if ((info.Length() == 0) || !(info[0].IsObject())) {
    return ERR(env, "invalid arguments", ErrorType::TypeError);
  }

  auto arg = info[0].ToObject();

  auto v_formats = arg.Get("formats");
  if (!v_formats.IsArray()) {
    return ERR(env, "invalid arguments", ErrorType::TypeError);
  }
  auto fmt_indices = getFormatIndices(v_formats.As<Napi::Array>());

  auto v_streams = arg.Get("streams");

  if (!v_streams.IsArray()) {
    return ERR(env, "invalid arguments", ErrorType::TypeError);
  }

  std::unique_ptr<UString> base_dir;
  auto v_base_dir = arg.Get("baseDir");
  if (v_base_dir.IsString()) {
    base_dir = ConvertNapiStringToUString(v_base_dir.ToString());
  }

  auto streams = std::make_unique<InStreams>(std::move(base_dir));
  auto r_append = streams->append_streams(v_streams.As<Napi::Array>());
  if (!r_append) {
    return ERR(env, "failed to create streams");
  }

  auto first_stream = streams->get_stream_by_index(0);
  if (!first_stream) {
    return ERR(env, "unexpected error");
  }

  std::unique_ptr<UString> password;
  auto v_password = arg.Get("password");
  if (v_password.IsString()) {
    password = ConvertNapiStringToUString(v_password.ToString());
  }

  CMyComPtr<OpenCallback> open_callback(new OpenCallback(std::move(streams), std::move(password)));

  for (auto i : fmt_indices) {
    CMyComPtr<IInArchive> archive;
    auto r = g_library_info->create_object(i, &IID_IInArchive, (void**)&archive);
    TRACE("fmt: %d, r: %d", i, r);
    if (r == S_OK) {
      auto r_open = archive->Open(first_stream, 0, open_callback);
      TRACE("r_open: %d", r_open);
      if (r_open == S_OK) {
        archive->Close();
        TRACE("after Close()");
      }
    }
  }

  TRACE("end createReader");

  return OK(env);
}

Napi::Object
InitReader(Napi::Env env, Napi::Object exports)
{
  exports.Set("createReader", Napi::Function::New(env, createReader));

  return exports;
}

} // namespace n7zip
