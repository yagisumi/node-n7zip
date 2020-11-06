#include "create_reader_worker.h"

namespace n7zip {
CreateReaderWorker::CreateReaderWorker(std::unique_ptr<CreateReaderArg>&& arg,
                                       Napi::Env env,
                                       Napi::Function func)
  : m_arg(std::move(arg))
{
  TRACE("+ CreateReaderWorker %p", this);

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    func,
    "CreateReaderWorker",
    0,
    1,
    this,
    CreateReaderWorker::Finalize,
    (void*)nullptr);

  m_thread = std::thread(&CreateReaderWorker::execute, this);
}

CreateReaderWorker::~CreateReaderWorker()
{
  TRACE("- CreateReaderWorker %p", this);
  m_thread.join();
}

void
CreateReaderWorker::abort(std::unique_ptr<error>&& err)
{
  TRACE("[CreateReaderWorker::abort]");
  m_err = std::move(err);
  auto r_status = m_tsfn.BlockingCall(this, CreateReaderWorker::InvokeCallback);
  TRACE("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
CreateReaderWorker::finish(std::unique_ptr<Reader>&& reader)
{
  TRACE("[CreateReaderWorker::finish]");
  m_reader = std::move(reader);
  auto r_status = m_tsfn.BlockingCall(this, CreateReaderWorker::InvokeCallback);
  TRACE("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
CreateReaderWorker::execute()
{
  auto streams = std::make_unique<InStreams>(std::move(m_arg->base_dir));
  auto err_append = streams->append_streams(std::move(m_arg->streams));
  if (err_append) {
    abort(std::move(err_append));
    return;
  }

  auto first_stream = streams->get_stream_by_index(0);
  if (!first_stream) {
    abort(std::make_unique<error>("Missing first stream"));
    return;
  }

  CMyComPtr<IArchiveOpenCallback> open_callback(
    new OpenCallback(std::move(streams), std::move(m_arg->password)));

  auto lock = g_library_info->get_shared_lock();

  for (auto i : m_arg->formats) {
    CMyComPtr<IInArchive> archive;
    auto r = g_library_info->create_object(i, &IID_IInArchive, (void**)&archive);
    TRACE("create_object > fmt: %d, r: %d", i, r);
    if (r == S_OK) {
      auto r_open = archive->Open(first_stream, 0, open_callback);
      TRACE("r_open: %d", r_open);
      if (r_open == S_OK) {
        finish(std::make_unique<Reader>(i, archive, open_callback));
        TRACE("finish");
        return;
      }
    }
  }

  abort(std::make_unique<error>("Failed to open archive"));
}

void
CreateReaderWorker::Finalize(Napi::Env, void*, CreateReaderWorker* self)
{
  TRACE("[CreateReaderWorker::Finalize]");
  delete self;
}

void
CreateReaderWorker::InvokeCallback(Napi::Env env,
                                   Napi::Function jsCallback,
                                   CreateReaderWorker* self)
{
  TRACE("[CreateReaderWorker::InvokeCallback]");
  if (self->m_err) {
    jsCallback.Call({ self->m_err->ERR(env) });
  } else if (!self->m_reader) {
    jsCallback.Call({ ERR(env, "Unexpected error") });
  } else {
    auto wrap_obj = ReaderWrap::constructor.New({});
    auto wrap = Napi::ObjectWrap<ReaderWrap>::Unwrap(wrap_obj);
    wrap->m_reader = std::move(self->m_reader);
    jsCallback.Call({ OK(env, wrap_obj) });
  }
}

} // namespace n7zip
