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
  static Napi::Object New(Napi::Env _env, const char* task_name);

  Napi::Value cancel(const Napi::CallbackInfo& info);
  Napi::Value taskName(const Napi::CallbackInfo& info);
};

} // namespace n7zip
