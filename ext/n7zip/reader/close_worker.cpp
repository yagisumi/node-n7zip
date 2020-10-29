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

  m_thread = std::thread(CloseWorker::Execute, this);
}

CloseWorker::~CloseWorker()
{
  TRACE("- CloseWorker %p", this);
  auto n = m_ref.Unref();
  TRACE("m_ref: %u", n);
  m_thread.join();
}

void
CloseWorker::Finalize(Napi::Env, void*, CloseWorker* self)
{
  TRACE("[CloseWorker::Finalize]");
  delete self;
}

void
CloseWorker::Invoke(Napi::Env env, Napi::Function jsCallback, void* value)
{
  auto result = (bool)value;
  if (result) {
    jsCallback.Call({ OK(env) });
  } else {
    jsCallback.Call({ ERR(env, "failed to close") });
  }
}

void
CloseWorker::Execute(CloseWorker* self)
{
  auto r_close = self->m_reader->close();
  auto r_status = self->m_tsfn.BlockingCall((void*)r_close, CloseWorker::Invoke);
  TRACE("napi_status: %d", r_status);
  self->m_tsfn.Release();
}

} // namespace n7zip
