#include "controller.h"

namespace n7zip {

Napi::FunctionReference Controller::constructor;

Napi::Object
Controller::Init(Napi::Env env, Napi::Object exports)
{
  auto func = DefineClass( //
    env,
    "Controller",
    {
      InstanceAccessor("taskName", &Controller::TaskName, nullptr),
      InstanceMethod("cancel", &Controller::Cancel),
      InstanceMethod("_pause", &Controller::Pause),
      InstanceMethod("_resume", &Controller::Resume),
    });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("Controller", func);

  return exports;
}

Napi::Object
Controller::New(Napi::Env, const char* task_name, bool paused)
{
  auto obj = constructor.New({});
  auto self = Napi::ObjectWrap<Controller>::Unwrap(obj);
  self->m_task_name = task_name;
  self->m_paused = paused;

  return obj;
}

Controller::Controller(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<Controller>(info)
{
  TRACE_THIS("+ Controller");
  m_canceled = false;
  m_paused = false;

  if (info.Length() > 0 && info[0].IsString()) {
    auto name = info[0].ToString().Utf8Value();
    m_task_name = name;
  }
}

Controller::~Controller()
{
  TRACE_THIS("- Controller");
}

Napi::Value
Controller::TaskName(const Napi::CallbackInfo& info)
{
  return Napi::String::New(info.Env(), m_task_name.c_str());
}

Napi::Value
Controller::Cancel(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Controller::Cancel]");
  cancel();

  return info.Env().Undefined();
}

Napi::Value
Controller::Pause(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Controller::Pause]");
  pause();

  return info.Env().Undefined();
}

Napi::Value
Controller::Resume(const Napi::CallbackInfo& info)
{
  TRACE_THIS("[Controller::Resume]");
  resume();

  return info.Env().Undefined();
}

void
Controller::cancel()
{
  TRACE_THIS("[Controller::cancel]");
  auto lock = acquire_lock();
  m_canceled = true;
  m_cv.notify_all();
}

bool
Controller::is_canceled()
{
  TRACE_THIS("[Controller::is_canceled]");
  auto lock = acquire_lock();
  return m_canceled;
}

void
Controller::pause()
{
  TRACE_THIS("[Controller::pause]");
  auto lock = acquire_lock();
  m_paused = true;
}

bool
Controller::is_paused()
{
  TRACE_THIS("[Controller::is_paused]");
  auto lock = acquire_lock();
  return m_paused;
}

void
Controller::resume()
{
  TRACE_THIS("[Controller::resume]");
  auto lock = acquire_lock();
  m_paused = false;
  m_cv.notify_all();
}

void
Controller::wait()
{
  TRACE_THIS("[Controller::wait]");
  auto lock = acquire_lock();
  if (!m_canceled && m_paused) {
    m_cv.wait(lock, [this]() { return !m_paused || m_canceled; });
  }
}

} // namespace n7zip
