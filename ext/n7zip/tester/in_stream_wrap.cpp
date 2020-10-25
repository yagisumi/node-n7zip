#include "in_stream_wrap.h"

namespace n7zip {

Napi::FunctionReference InStreamWrap::constructor;

Napi::Object
InStreamWrap::Init(Napi::Env env, Napi::Object tester)
{
  auto func = DefineClass( //
    env,
    "InStreamWrap",
    {
      InstanceMethod("seek", &InStreamWrap::Seek),
      InstanceMethod("read", &InStreamWrap::Read),
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  return tester;
}

InStreamWrap::InStreamWrap(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<InStreamWrap>(info)
{}

Napi::Value
InStreamWrap::Seek(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  if (info.Length() < 2 || !(info[0].IsNumber()) || !(info[1].IsNumber())) {
    return ERR(env, "invalid arguments");
  }

  if (!m_inStream) {
    return ERR(env, "uninitialized InStream");
  }

  auto offset = info[0].ToNumber().Int64Value();
  auto seek_origin = info[1].ToNumber().Int32Value();

  UInt64 new_position;
  auto r = m_inStream->Seek(offset, seek_origin, &new_position);

  if (r != S_OK) {
    return ERR(env, "seek error");
  } else {
    return OK(env, Napi::Number::New(env, new_position));
  }
}

Napi::Value
InStreamWrap::Read(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  if (info.Length() < 1 || !(info[0].IsNumber())) {
    return ERR(env, "invalid argument");
  }

  auto size = info[0].ToNumber().Int32Value();
  if (size < 0) {
    return ERR(env, "invalid argument");
  }

  if (!m_inStream) {
    return ERR(env, "uninitialized InStream");
  }

  auto buf = std::make_unique<char[]>(size);
  UInt32 processed_size = 0;
  auto r = m_inStream->Read(buf.get(), size, &processed_size);

  if (r != S_OK) {
    return ERR(env, "read error");
  } else {
    return OK(env, Napi::Buffer<char>::Copy(env, buf.get(), processed_size));
  }
}

static FdInStream*
createFdInStream(Napi::Object arg)
{
  auto fd = arg.Get("source").ToNumber();
  auto AutoClose = true;
  auto auto_close = arg.Get("AutoClose");
  if (auto_close.IsBoolean()) {
    AutoClose = auto_close.ToBoolean().Value();
  }

  return new FdInStream(fd, AutoClose);
}

static FdInStream*
createFdInStreamFromPath(Napi::Object arg)
{
  auto path = arg.Get("source").ToString().Utf8Value();
  auto AutoClose = true;

  uv_fs_t open_req;
  auto r = uv_fs_open(nullptr, &open_req, path.c_str(), UV_FS_O_RDONLY, 0666, nullptr);
  if (r < 0) {
    return nullptr;
  }

  return new FdInStream(r, AutoClose);
}

static BufferInStream*
createBufferInStream(Napi::Object arg)
{
  auto buf = arg.Get("source").As<Napi::Buffer<char>>();
  auto ShareBuffer = false;
  auto share_buffer = arg.Get("ShareBuffer");
  if (share_buffer.IsBoolean()) {
    ShareBuffer = share_buffer.ToBoolean().Value();
  }

  return new BufferInStream(buf, ShareBuffer);
}

static Napi::Value
createInStream(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  if (info.Length() < 1 || !(info[0].IsObject())) {
    return ERR(env, "InvalidArgument");
  }

  auto arg = info[0].ToObject();
  auto source = arg.Get("source");
  auto name = arg.Get("name");
  if (!(name.IsString())) {
    return ERR(env, "InvalidArgument");
  }

  CMyComPtr<IInStream> stream;

  if (source.IsArray()) {
    // MultiInStream
    return ERR(env, "NotImplemented");
  } else if (source.IsNumber()) {
    // FdInStream
    stream = createFdInStream(arg);
  } else if (source.IsString()) {
    // FdInStream
    stream = createFdInStreamFromPath(arg);
    if (!stream) {
      return ERR(env, "FailedToOpenFile");
    }
  } else if (source.IsBuffer()) {
    // BufferInStream
    stream = createBufferInStream(arg);
  } else {
    return ERR(env, "InvalidArgument");
  }

  if (!stream) {
    return ERR(env, "UnexpectedError");
  }

  auto in_stream_obj = InStreamWrap::constructor.New({});
  auto in_stream_wrap = Napi::ObjectWrap<InStreamWrap>::Unwrap(in_stream_obj);
  in_stream_wrap->m_inStream = stream;

  return OK(env, in_stream_obj);
}

static Napi::Value
createInStream_(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  if (info.Length() < 1) {
    return ERR(env, "invalid argument");
  }

  if (info[0].IsNumber()) {
#ifdef _WIN32
    auto fd = info[0].ToNumber().Int32Value();
#else
    auto fd = info[0].ToNumber().Int64Value();
#endif
    if (fd < 0) {
      return ERR(env, "invalid file descriptor");
    }

    bool autoclose = true;
    if (info.Length() > 1 && info[1].IsBoolean()) {
      autoclose = info[1].ToBoolean().Value();
    }

    CMyComPtr<FdInStream> in_stream(new FdInStream(fd, autoclose));
    auto in_stream_obj = InStreamWrap::constructor.New({});
    auto in_stream_wrap = Napi::ObjectWrap<InStreamWrap>::Unwrap(in_stream_obj);
    in_stream_wrap->m_inStream = in_stream;

    return OK(env, in_stream_obj);
  } else if (info[0].IsString()) {
    auto str = info[0].ToString();
    auto path = str.Utf8Value();
    uv_fs_t open_req;
    auto r = uv_fs_open(nullptr, &open_req, path.c_str(), UV_FS_O_RDONLY, 0666, nullptr);
    // r == open_req.result
    if (r < 0) {
      return ERR(env, "file open error");
    } else {
      CMyComPtr<FdInStream> in_stream(new FdInStream(r, true));
      auto in_stream_obj = InStreamWrap::constructor.New({});
      auto in_stream_wrap = Napi::ObjectWrap<InStreamWrap>::Unwrap(in_stream_obj);
      in_stream_wrap->m_inStream = in_stream;

      return OK(env, in_stream_obj);
    }
  } else {
    return ERR(env, "invalid argument");
  }
}

Napi::Object
InitInStreamWrap(Napi::Env env, Napi::Object tester)
{
  InStreamWrap::Init(env, tester);
  tester.Set("createInStream", Napi::Function::New(env, createInStream));

  return tester;
}

} // namespace n7zip
