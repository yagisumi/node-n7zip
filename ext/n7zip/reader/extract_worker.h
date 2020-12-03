#pragma once

#include "../common.h"
#include "reader.h"
#include "../callbacks/extract_callback.h"
#include "../streams/out_stream.h"

namespace n7zip {

class ExtractWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  Reader* m_reader;
  UInt32 m_index;
  UInt32 m_limit;
  bool m_test_mode;
  CMyComPtr<ExtractCallback> m_extract_callback;

public:
  ExtractWorker(Napi::Env env,
                Napi::Function callback,
                Reader* reader,
                UInt32 index,
                UInt32 limit,
                bool test_mode);
  ~ExtractWorker();

  void execute();
  void postBuffer(std::unique_ptr<OutBufferData>&& buffer);

  static void Finalize(Napi::Env, void*, ExtractWorker* self);
  static void InvokeCallback(Napi::Env env, Napi::Function jsCallback, ExtractWorker* self);
};

} // namespace n7zip
