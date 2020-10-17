#include "test_object.h"

#ifdef DEBUG

namespace n7zip {

Napi::FunctionReference SharedLocker::constructor;

Napi::Object
SharedLocker::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func =
    DefineClass(env, "SharedLocker", { InstanceMethod("run", &SharedLocker::Run) });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("SharedLocker", func);
  return exports;
}

SharedLocker::SharedLocker(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<SharedLocker>(info)
{
  TRACE("+ SharedLocker %p", this);
  auto self = info.This();
  m_self = Napi::Reference<Napi::Value>::New(self);
}

SharedLocker::~SharedLocker()
{
  TRACE("- SharedLocker %p", this);
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
    auto callback = [](Napi::Env env, Napi::Function jsCallback) { jsCallback.Call({}); };

    auto lock = g_library_info->GetSharedLock();
    std::this_thread::sleep_for(std::chrono::seconds(3));
    auto status = this->m_context->tsfn.BlockingCall(callback);

    this->m_context->tsfn.Release();
  });

  m_self.Ref();
  m_lock = std::move(lock);

  return OK(env);
}

Napi::Object
InitTestObject(Napi::Env env, Napi::Object exports)
{
  SharedLocker::Init(env, exports);

  return exports;
}

} // namespace n7zip

#endif
