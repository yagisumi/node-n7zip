#include "canceler.h"

namespace n7zip {

Napi::FunctionReference Canceler::constructor;

Napi::Object
Canceler::Init(Napi::Env env, Napi::Object exports)
{
  auto func = DefineClass( //
    env,
    "Canceler",
    {
      InstanceMethod("cancel", &Canceler::Cancel),
      InstanceAccessor("taskName", &Canceler::TaskName, nullptr),
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  return exports;
}

Napi::Object
Canceler::New(Napi::Env _env, const char* task_name)
{
  auto obj = constructor.New({});
  auto self = Napi::ObjectWrap<Canceler>::Unwrap(obj);
  self->m_task_name = task_name;

  return obj;
}

Canceler::Canceler(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<Canceler>(info)
{
  TRACE_P("+ Canceler");
  m_canceled.store(false);
}

Canceler::~Canceler()
{
  TRACE_P("- Canceler");
}

Napi::Value
Canceler::Cancel(const Napi::CallbackInfo& info)
{
  m_canceled.store(true);

  return info.Env().Undefined();
}

Napi::Value
Canceler::TaskName(const Napi::CallbackInfo& info)
{
  return Napi::String::New(info.Env(), m_task_name.c_str());
}

} // namespace n7zip
