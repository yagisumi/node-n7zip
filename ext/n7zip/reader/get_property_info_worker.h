#pragma once

#include "../common.h"
#include "reader.h"

namespace n7zip {

class GetPropertyInfoWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  Reader* m_reader;
  std::unique_ptr<ReaderPropertyInfo> m_info;

public:
  GetPropertyInfoWorker(Reader* reader, Napi::Env env, Napi::Function callback);
  ~GetPropertyInfoWorker();

  void execute();

  static void Finalize(Napi::Env, void*, GetPropertyInfoWorker* self);
  static void InvokeCallback(Napi::Env env, Napi::Function jsCallback, GetPropertyInfoWorker* self);
};

} // namespace n7zip
