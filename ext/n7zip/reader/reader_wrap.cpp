#include "reader_wrap.h"

namespace n7zip {

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
  TRACE("+ ReaderWrap %p", this);
}

ReaderWrap::~ReaderWrap()
{
  TRACE("- ReaderWrap %p", this);
}

Napi::Value
ReaderWrap::isClosed(const Napi::CallbackInfo& info)
{
  TRACE("[ReaderWrap::isClosed]");
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
  TRACE("[ReaderWrap::close]");
  auto env = info.Env();

  if (!m_reader) {
    return ERR(env, "invalid reader");
  }

  if (info.Length() == 0 || !(info[0].IsFunction())) {
    return ERR(env, "InvalidArgs");
  }

  auto callback = info[0].As<Napi::Function>();

  if (m_reader->m_closed.load()) {
    callback.Call({ OK(env) });
    return OK(env);
  }

  auto self = info.This().ToObject();
  new CloseWorker(m_reader.get(), env, self, callback);

  return OK(env);
}

} // namespace n7zip
