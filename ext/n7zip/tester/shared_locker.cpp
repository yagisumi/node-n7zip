#ifdef DEBUG

  #include "shared_locker.h"
  #include "../library.h"

namespace n7zip {

Napi::FunctionReference SharedLocker::constructor;

Napi::Object
SharedLocker::Init(Napi::Env env, Napi::Object tester)
{
  Napi::Function func =
    DefineClass(env, "SharedLocker", { InstanceMethod("run", &SharedLocker::Run) });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  tester.Set("SharedLocker", func);
  return tester;
}

SharedLocker::SharedLocker(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<SharedLocker>(info)
{
  TRACE_P("+ SharedLocker");
  auto self = info.This();
  m_self = Napi::Reference<Napi::Value>::New(self);
}

SharedLocker::~SharedLocker()
{
  TRACE_P("- SharedLocker");
}

Napi::Value
SharedLocker::Run(const Napi::CallbackInfo& info)
{
  auto env = info.Env();

  auto lock = TryLock();
  if (!lock.owns_lock()) {
    return ERR(env, "already running");
  }

  if (info.Length() < 1 || !info[0].IsFunction()) {
    return ERR(env, "expected one callback function");
  }

  auto cb = info[0].As<Napi::Function>();

  m_context.reset(new SharedLockerContext());

  m_context->tsfn = Napi::ThreadSafeFunction::New( //
    env,
    cb,
    "SharedLocker",
    0,
    1,
    [this](Napi::Env) {
      this->m_self.Unref();
      this->m_lock.unlock();
      this->m_context->native_thread.join();
      this->m_context = nullptr;
    });

  m_context->native_thread = std::thread([this]() {
    auto callback = [](Napi::Env, Napi::Function jsCallback) { jsCallback.Call({}); };

    std::this_thread::sleep_for(std::chrono::seconds(2));
    this->m_context->tsfn.BlockingCall(callback);

    this->m_context->tsfn.Release();
  });

  m_self.Ref();
  m_lock = std::move(lock);
  m_context->lock = g_library_info->get_shared_lock();

  return OK(env);
}

Napi::Object
InitSharedLocker(Napi::Env env, Napi::Object tester)
{
  SharedLocker::Init(env, tester);

  return tester;
}

} // namespace n7zip

#endif
