#pragma once

#ifdef DEBUG

  #include "n7zip/common.h"
  #include <chrono>

namespace n7zip {

struct SharedLockerContext
{
  SharedLockerContext() { TRACE("+ SharedLockerContext %p", this); }
  ~SharedLockerContext() { TRACE("- SharedLockerContext %p", this); }
  std::thread native_thread;
  Napi::ThreadSafeFunction tsfn;
  std::shared_lock<std::shared_timed_mutex> lock;
};

class SharedLocker : public Napi::ObjectWrap<SharedLocker>
{
public:
  static Napi::FunctionReference constructor;
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  SharedLocker(const Napi::CallbackInfo& info);
  ~SharedLocker();
  Napi::Value Run(const Napi::CallbackInfo& info);
  std::unique_ptr<SharedLockerContext> m_context;
  std::unique_lock<std::mutex> m_lock;
  Napi::Reference<Napi::Value> m_self;

private:
  std::mutex m_mutex;
  std::unique_lock<std::mutex> TryLock()
  {
    return std::unique_lock<std::mutex>(m_mutex, std::try_to_lock);
  }
};

Napi::Object
InitSharedLocker(Napi::Env env, Napi::Object exports);
} // namespace n7zip

#endif
