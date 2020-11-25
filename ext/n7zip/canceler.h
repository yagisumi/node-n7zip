#pragma once

#include "common.h"

namespace n7zip {

class Canceler : public Napi::ObjectWrap<Canceler>
{
public:
  std::atomic<bool> m_canceled;
  std::string m_task_name;
  Canceler(const Napi::CallbackInfo& info);
  ~Canceler();
  static Napi::FunctionReference constructor;
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env, const char* task_name);

  Napi::Value Cancel(const Napi::CallbackInfo& info);
  Napi::Value TaskName(const Napi::CallbackInfo& info);

  inline void cancel() { m_canceled.store(true); }
  inline bool is_canceled() { return m_canceled.load(); }
};

} // namespace n7zip
