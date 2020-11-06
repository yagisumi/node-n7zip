#include "close_worker.h"

namespace n7zip {

CloseWorker::CloseWorker(Reader* reader, Napi::Env env, Napi::Object wrap, Napi::Function func)
  : m_reader(reader)
{
  TRACE("+ CloseWorker %p", this);
  m_ref = Napi::Persistent(wrap);

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    func,
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
  TRACE("- CloseWorker %p", this);
  auto n = m_ref.Unref();
  TRACE("m_ref: %u", n);
  m_thread.join();
}

void
CloseWorker::execute()
{
  m_result = m_reader->close();
  auto r_status = m_tsfn.BlockingCall(this, CloseWorker::InvokeCallback);
  TRACE("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
CloseWorker::Finalize(Napi::Env, void*, CloseWorker* self)
{
  TRACE("[CloseWorker::Finalize]");
  delete self;
}

void
CloseWorker::InvokeCallback(Napi::Env env, Napi::Function jsCallback, CloseWorker* self)
{
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
