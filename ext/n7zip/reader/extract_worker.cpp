#include "extract_worker.h"

namespace n7zip {

ExtractWorker::ExtractWorker(Napi::Env env,
                             Napi::Function callback,
                             Reader* reader,
                             UInt32 index,
                             UInt32 limit,
                             bool test_mode,
                             Controller* controller)
  : m_reader(reader)
  , m_index(index)
  , m_limit(limit)
  , m_test_mode(test_mode)
  , m_controller(controller)
{
  TRACE_THIS("+ ExtractWorker");
  auto n = m_reader->Ref();
  TRACE_THIS("m_reader->Ref(): %u", n);
  if (m_controller) {
    m_controller->Ref();
  }

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
  if (m_controller) {
    m_controller->Unref();
  }
  m_thread.join();
}

void
ExtractWorker::execute()
{
  TRACE_THIS("[ExtractWorker::execute]");
  auto lock = m_reader->acquire_lock();
  if (m_reader->is_closed()) {
    auto r_status = m_tsfn.BlockingCall([this](Napi::Env env, Napi::Function jsCallback) {
      TRACE_THIS("[ExtractWorker::execute/ErrorResponse]");
      try {
        auto response = Napi::Object::New(env);
        response.Set("message", Napi::String::New(env, "Reader is already closed"));
        jsCallback.Call({ response });
      } catch (...) {
      }
    });
    TRACE_THIS("napi_status: %d", r_status);
  }

  auto is_dir = m_reader->is_dir(m_index);
  auto is_link = m_reader->is_link(m_index);
  if (is_dir || is_link) {
    auto r_status =
      m_tsfn.BlockingCall([this, is_dir, is_link](Napi::Env env, Napi::Function jsCallback) {
        TRACE_THIS("[ExtractWorker::execute/EndResponse/skip]");
        auto response = Napi::Object::New(env);
        response.Set("type", Napi::String::New(env, "end"));
        response.Set("skipExtraction", Napi::Boolean::New(env, true));
        response.Set("isDir", Napi::Boolean::New(env, is_dir));
        response.Set("isLink", Napi::Boolean::New(env, is_link));
        try {
          jsCallback.Call({ response });
        } catch (...) {
        }
      });
    TRACE_THIS("napi_status: %d", r_status);
  } else {
    m_extract_callback = new ExtractCallback(this, m_limit);
    auto r_extract = m_reader->extract(&m_index, 1, m_test_mode, m_extract_callback);
    TRACE_THIS("r_extract: %d", r_extract);
    auto r_status =
      m_tsfn.BlockingCall([this, r_extract](Napi::Env env, Napi::Function jsCallback) {
        TRACE_THIS("[ExtractWorker::execute/EndResponse/extract]");
        auto response = Napi::Object::New(env);
        response.Set("type", Napi::String::New(env, "end"));
        response.Set("skipExtraction", Napi::Boolean::New(env, false));
        response.Set("extractResult", Napi::Number::New(env, r_extract));
        try {
          jsCallback.Call({ response });
        } catch (...) {
        }
      });
    TRACE_THIS("napi_status: %d", r_status);
  }

  m_tsfn.Release();
}

void
ExtractWorker::post_buffer(std::unique_ptr<OutBufferData>&& buffer)
{
  TRACE_THIS("[ExtractWorker::post_buffer]");
  wait();
  auto buffer_ptr = buffer.release();

  auto r = m_tsfn.BlockingCall(
    buffer_ptr, [this](Napi::Env env, Napi::Function jsCallback, OutBufferData* buffer_ptr) {
      TRACE_THIS("[ExtractWorker::post_buffer/BufferResponse]");
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

bool
ExtractWorker::is_canceled()
{
  return m_controller && m_controller->is_canceled();
}

void
ExtractWorker::wait()
{
  if (m_controller) {
    m_controller->wait();
  }
}

} // namespace n7zip
