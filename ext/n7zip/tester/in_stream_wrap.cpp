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

static Napi::Value
createInStream(const Napi::CallbackInfo& info)
{
  auto env = info.Env();
  if (info.Length() < 1 || info[0].IsArray() || !info[0].IsObject()) {
    return ERR(env, "The argument must be Object");
  }

  auto obj = info[0].ToObject();
  auto r_arg = buildInStreamArg(obj, true);
  if (r_arg.err()) {
    return r_arg.err()->ERR(env);
  } else if (!r_arg.ok()) {
    return ERR(env, "Unexpected error");
  }

  auto arg = r_arg.move_ok();

  auto r_stream = arg->createInStream();
  if (r_stream.err()) {
    return r_stream.err()->ERR(env);
  } else if (!r_stream.ok()) {
    return ERR(env, "Unexpected error");
  }

  CMyComPtr<IInStream> stream = r_stream.release_ok();

  auto in_stream_obj = InStreamWrap::constructor.New({});
  auto in_stream_wrap = Napi::ObjectWrap<InStreamWrap>::Unwrap(in_stream_obj);
  in_stream_wrap->m_inStream = stream;

  return OK(env, in_stream_obj);
}

Napi::Object
InitInStreamWrap(Napi::Env env, Napi::Object tester)
{
  InStreamWrap::Init(env, tester);
  tester.Set("createInStream", Napi::Function::New(env, createInStream));

  return tester;
}

} // namespace n7zip
