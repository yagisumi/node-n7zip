#pragma once

#include "common.h"
#include <condition_variable>

namespace n7zip {

class Controller : public Napi::ObjectWrap<Controller>
{
  std::mutex m_mutex;
  std::condition_variable m_cv;
  bool m_paused;
  bool m_canceled;
  std::string m_task_name;
  inline std::unique_lock<std::mutex> acquire_lock()
  {
    return std::unique_lock<std::mutex>(m_mutex);
  }

public:
  Controller(const Napi::CallbackInfo& info);
  ~Controller();
  static Napi::FunctionReference constructor;
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env, const char* task_name, bool paused);

  Napi::Value TaskName(const Napi::CallbackInfo& info);
  Napi::Value Cancel(const Napi::CallbackInfo& info);
  Napi::Value Pause(const Napi::CallbackInfo& info);
  Napi::Value Resume(const Napi::CallbackInfo& info);

  void cancel();
  bool is_canceled();
  void pause();
  bool is_paused();
  void resume();
  void wait();
};

} // namespace n7zip
