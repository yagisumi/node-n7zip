#pragma once

#include "../common.h"
#include "reader.h"
#include "../callbacks/extract_callback.h"
#include "../streams/out_buffer_data.h"
#include "../controller.h"

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
  Controller* m_controller;

public:
  ExtractWorker(Napi::Env env,
                Napi::Function callback,
                Reader* reader,
                UInt32 index,
                UInt32 limit,
                bool test_mode,
                Controller* controller);
  ~ExtractWorker();

  static void Finalize(Napi::Env, void*, ExtractWorker* self);

  bool is_canceled();
  void post_buffer(std::unique_ptr<OutBufferData>&& buffer);

private:
  void execute();
  void wait();
};

} // namespace n7zip
