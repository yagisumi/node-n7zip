#include "create_reader_worker.h"
#include "../library.h"
#include "../streams/in_streams.h"
#include "../callbacks/open_callback.h"

namespace n7zip {
CreateReaderWorker::CreateReaderWorker(Napi::Env env,
                                       Napi::Function callback,
                                       std::unique_ptr<CreateReaderArg>&& arg)
  : m_arg(std::move(arg))
{
  TRACE_THIS("+ CreateReaderWorker");

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    callback,
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
  TRACE_THIS("- CreateReaderWorker");
  m_thread.join();
}

void
CreateReaderWorker::abort(std::unique_ptr<error>&& err)
{
  TRACE_THIS("[CreateReaderWorker::abort]");
  m_err = std::move(err);
  auto r_status = m_tsfn.BlockingCall(this, CreateReaderWorker::InvokeCallback);
  TRACE_THIS("napi_status: %d", r_status);
  m_tsfn.Release();
}

void
CreateReaderWorker::finish(int fmt_index,
                           CMyComPtr<IInArchive>& archive,
                           CMyComPtr<IArchiveOpenCallback>& open_callback)
{
  TRACE_THIS("[CreateReaderWorker::finish]");
  m_reader_args.fmt_index = fmt_index;
  m_reader_args.archive = archive;
  m_reader_args.open_callback = open_callback;
  auto r_status = m_tsfn.BlockingCall(this, CreateReaderWorker::InvokeCallback);
  TRACE_THIS("napi_status: %d", r_status);
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
    TRACE_THIS("create_object > fmt: %d, r: %d", i, r);
    if (r == S_OK) {
      TRACE_PTR((IInArchive*)archive, "@ IInArchive");
      auto r_open = archive->Open(first_stream, 0, open_callback);
      TRACE_THIS("r_open: %d", r_open);
      if (r_open == S_OK) {
        finish(i, archive, open_callback);
        return;
      }
    }
  }

  abort(std::make_unique<error>("Failed to open archive"));
}

void
CreateReaderWorker::Finalize(Napi::Env, void*, CreateReaderWorker* self)
{
  TRACE_PTR(self, "[CreateReaderWorker::Finalize]");
  delete self;
}

void
CreateReaderWorker::InvokeCallback(Napi::Env env,
                                   Napi::Function jsCallback,
                                   CreateReaderWorker* self)
{
  TRACE_PTR(self, "[CreateReaderWorker::InvokeCallback]");
  try {
    if (self->m_err) {
      jsCallback.Call({ self->m_err->ERR(env) });
    } else if (!self->m_reader_args.archive || !self->m_reader_args.open_callback) {
      jsCallback.Call({ ERR(env, "Unexpected error") });
    } else {
      auto reader_obj = Reader::New(env,
                                    self->m_reader_args.fmt_index,
                                    self->m_reader_args.archive,
                                    self->m_reader_args.open_callback);
      jsCallback.Call({ OK(env, reader_obj) });
    }
  } catch (...) {
    TRACE_PTR(self, "[CreateReaderWorker::InvokeCallback] catch ...");
  }
}

} // namespace n7zip
