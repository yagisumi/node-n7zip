#pragma once

#include "../common.h"
#include "reader.h"

namespace n7zip {

class CloseWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  Napi::ObjectReference m_ref;
  std::thread m_thread;
  Reader* m_reader;

public:
  CloseWorker(Reader* reader, Napi::Env env, Napi::Object wrap, Napi::Function func);
  ~CloseWorker();

  void execute();

  static void Finalize(Napi::Env, void*, CloseWorker* self);
  static void InvokeCallback(Napi::Env env, Napi::Function jsCallback, void* value);
};

} // namespace n7zip
