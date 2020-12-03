#include "extract_worker.h"

namespace n7zip {

ExtractWorker::ExtractWorker(Napi::Env env,
                             Napi::Function callback,
                             Reader* reader,
                             UInt32 index,
                             UInt32 limit,
                             bool test_mode)
  : m_reader(reader)
  , m_index(index)
  , m_limit(limit)
  , m_test_mode(test_mode)
{
  TRACE_THIS("+ ExtractWorker");
  auto n = m_reader->Ref();
  TRACE_THIS("m_reader->Ref(): %u", n);

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    callback,
    "ExtractWorker",
    10,
    1,
    this,
    ExtractWorker::Finalize,
    (void*)nullptr);

  m_thread = std::thread(&ExtractWorker::execute, this);
}

ExtractWorker::~ExtractWorker()
{
  TRACE_THIS("- ExtractWorker");
  auto n = m_reader->Unref();
  TRACE_THIS("m_reader->Unref(): %u", n);
  m_thread.join();
}

void
ExtractWorker::execute()
{
  auto lock = m_reader->lock();
  // bool is_dir = false;
  // auto r_dir = m_reader->is_dir(m_index, is_dir);
  m_extract_callback = new ExtractCallback(this, m_limit);
  auto r_extract = m_reader->extract(&m_index, 1, m_test_mode, m_extract_callback);
  TRACE_THIS("r_extract: %d", r_extract);
  auto r_status = m_tsfn.BlockingCall(this, ExtractWorker::InvokeCallback);
  TRACE_THIS("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
ExtractWorker::postBuffer(std::unique_ptr<OutBufferData>&& buffer)
{
  TRACE_THIS("[ExtractWorker::postBuffer]");
  auto buffer_ptr = buffer.release();

  auto r = m_tsfn.BlockingCall(
    buffer_ptr, [this](Napi::Env env, Napi::Function jsCallback, OutBufferData* buffer_ptr) {
      TRACE_THIS("[ExtractWorker::postBuffer/jsCallback]");
      std::unique_ptr<OutBufferData> buffer(buffer_ptr);
      auto result = buffer->createResult(env);
      try {
        jsCallback.Call({ result });
      } catch (...) {
      }
    });

  if (r != napi_ok) {
    delete buffer_ptr;
  }
}

void
ExtractWorker::Finalize(Napi::Env, void*, ExtractWorker* self)
{
  TRACE_PTR(self, "[ExtractWorker::Finalize]");
  delete self;
}

void
ExtractWorker::InvokeCallback(Napi::Env env, Napi::Function jsCallback, ExtractWorker* self)
{
  TRACE_PTR(self, "[ExtractWorker::InvokeCallback]");
  try {
    jsCallback.Call({ OK(env) });
  } catch (...) {
  }
}

} // namespace n7zip
