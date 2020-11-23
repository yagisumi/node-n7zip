#include "close_worker.h"

namespace n7zip {

CloseWorker::CloseWorker(Napi::Env env, Napi::Function callback, Reader* reader)
  : m_reader(reader)
{
  TRACE_THIS("+ CloseWorker");
  auto n = m_reader->Ref();
  TRACE_THIS("m_reader->Ref(): %u", n);

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    callback,
    "CloseWorker",
    0,
    1,
    this,
    CloseWorker::Finalize,
    (void*)nullptr);

  m_thread = std::thread(&CloseWorker::execute, this);
}

CloseWorker::~CloseWorker()
{
  TRACE_THIS("- CloseWorker");
  auto n = m_reader->Unref();
  TRACE_THIS("m_reader->Unref(): %u", n);
  m_thread.join();
}

void
CloseWorker::execute()
{
  m_result = m_reader->close();
  auto r_status = m_tsfn.BlockingCall(this, CloseWorker::InvokeCallback);
  TRACE_THIS("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
CloseWorker::Finalize(Napi::Env, void*, CloseWorker* self)
{
  TRACE_PTR(self, "[CloseWorker::Finalize]");
  delete self;
}

void
CloseWorker::InvokeCallback(Napi::Env env, Napi::Function jsCallback, CloseWorker* self)
{
  TRACE_PTR(self, "[CloseWorker::InvokeCallback]");
  try {
    if (self->m_result == S_OK) {
      jsCallback.Call({ OK(env) });
    } else {
      jsCallback.Call({ ERR(env, "Failed to close (HRESULT: %d)", self->m_result) });
    }
  } catch (...) {
  }
}

} // namespace n7zip
