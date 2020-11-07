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
      InstanceMethod("getNumberOfItems", &ReaderWrap::getNumberOfItems),
      InstanceMethod("getNumberOfArchiveProperties", &ReaderWrap::getNumberOfArchiveProperties),
      InstanceMethod("getNumberOfProperties", &ReaderWrap::getNumberOfProperties),
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
  TRACE_P("+ ReaderWrap");
}

ReaderWrap::~ReaderWrap()
{
  TRACE_P("- ReaderWrap");
}

Napi::Value
ReaderWrap::getNumberOfItems(const Napi::CallbackInfo& info)
{
  TRACE_P("[ReaderWrap::getNumberOfItems]");
  auto env = info.Env();
  if (m_reader) {
    return Napi::Number::New(env, m_reader->m_num_of_items);
  } else { // unexpected
    return Napi::Number::New(env, 0);
  }
}

Napi::Value
ReaderWrap::getNumberOfArchiveProperties(const Napi::CallbackInfo& info)
{
  TRACE_P("[ReaderWrap::getNumberOfArchiveProperties]");
  auto env = info.Env();
  if (m_reader) {
    return Napi::Number::New(env, m_reader->m_num_of_arc_props);
  } else { // unexpected
    return Napi::Number::New(env, 0);
  }
}

Napi::Value
ReaderWrap::getNumberOfProperties(const Napi::CallbackInfo& info)
{
  TRACE_P("[ReaderWrap::getNumberOfProperties]");
  auto env = info.Env();
  if (m_reader) {
    return Napi::Number::New(env, m_reader->m_num_of_props);
  } else { // unexpected
    return Napi::Number::New(env, 0);
  }
}

Napi::Value
ReaderWrap::isClosed(const Napi::CallbackInfo& info)
{
  TRACE_P("[ReaderWrap::isClosed]");
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
  TRACE_P("[ReaderWrap::close]");
  auto env = info.Env();

  if (!m_reader) {
    return ERR(env, "Uninitialized Reader");
  }

  if (info.Length() == 0 || !(info[0].IsFunction())) {
    return ERR(env, "No callback function was given");
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
