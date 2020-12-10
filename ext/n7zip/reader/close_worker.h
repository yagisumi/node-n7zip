#pragma once

#include "../common.h"
#include "reader.h"

namespace n7zip {

class CloseWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  Reader* m_reader;

public:
  CloseWorker(Napi::Env env, Napi::Function callback, Reader* reader);
  ~CloseWorker();

  void execute();

  static void Finalize(Napi::Env, void*, CloseWorker* self);
};

} // namespace n7zip
