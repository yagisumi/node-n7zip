#include "get_entries_worker.h"
#include <algorithm>
#include <chrono>

namespace n7zip {

GetEntriesWorker::GetEntriesWorker(Napi::Env env,
                                   Napi::Function callback,
                                   Reader* reader,
                                   GetEntriesWorkerArgs&& args,
                                   Canceler* canceler)
  : m_reader(reader)
  , m_args(std::move(args))
  , m_canceler(canceler)
{
  TRACE_THIS("+ GetEntriesWorker");
  auto n = m_reader->Ref();
  TRACE_THIS("m_reader->Ref(): %u", n);
  m_canceler->Ref();

  m_tsfn = Napi::ThreadSafeFunction::New( //
    env,
    callback,
    "GetEntriesWorker",
    0,
    1,
    this,
    GetEntriesWorker::Finalize,
    (void*)nullptr);

  m_thread = std::thread(&GetEntriesWorker::execute, this);
}

GetEntriesWorker::~GetEntriesWorker()
{
  TRACE_THIS("- GetEntriesWorker");
  auto n = m_reader->Unref();
  TRACE_THIS("m_reader->Unref(): %u", n);
  m_canceler->Unref();
  m_thread.join();
}

void
GetEntriesWorker::execute()
{
  TRACE_THIS("[GetEntriesWorker::execute]");
  auto lock = m_reader->lock();

  for (UInt32 i = m_args.start; i < m_args.end; i += m_args.limit) {
    if (m_canceler->m_canceled.load()) {
      auto r_status = m_tsfn.BlockingCall(this, GetEntriesWorker::InvokeCallbackERR);
      TRACE_THIS("napi_status: %d", r_status);
      break;
    }

    auto end = std::min(i + m_args.limit, m_args.end);
    TRACE_THIS("[GetEntriesWorker::execute] i: %u, end: %u", i, end);

    auto done = end == m_args.end;
    auto message_ptr =
      new GetEntriesMessage(m_reader->get_entries(i, end, m_args.prop_ids), done, this);

    auto r_status = m_tsfn.BlockingCall(message_ptr, GetEntriesWorker::InvokeCallbackOK);
    TRACE_THIS("napi_status: %d", r_status);
    if (r_status != napi_ok) {
      delete message_ptr;
    }
  }

  m_tsfn.Release();
}

void
GetEntriesWorker::Finalize(Napi::Env, void*, GetEntriesWorker* self)
{
  TRACE_PTR(self, "[GetEntriesWorker::Finalize]");
  delete self;
}

void
GetEntriesWorker::InvokeCallbackOK(Napi::Env env,
                                   Napi::Function jsCallback,
                                   GetEntriesMessage* message_ptr)
{
  std::unique_ptr<GetEntriesMessage> message(message_ptr);
  TRACE_PTR(message->worker, "[GetEntriesWorker::InvokeCallback]");
#ifdef DEBUG
  auto start = std::chrono::system_clock::now();
#endif

  try {
    auto length = message->entries.size();
    auto entries = Napi::Array::New(env, length);
    for (size_t i = 0; i < length; i++) {
      auto& entry = message->entries[i];
      auto entry_obj = Napi::Object::New(env);

      auto props = Napi::Array::New(env, entry.props.size());

      for (size_t pidx = 0; pidx < entry.props.size(); pidx++) {
        auto& prop = entry.props[pidx];
        auto prop_obj = Napi::Object::New(env);
        prop_obj.Set("id", Napi::Number::New(env, prop.prop_id));
        prop_obj.Set("value", ConvertPropVariant(env, prop.prop));
        props.Set(pidx, prop_obj);
      }

      entry_obj.Set("index", Napi::Number::New(env, entry.index));
      entry_obj.Set("props", props);

      entries.Set(i, entry_obj);
    }

    auto value = Napi::Object::New(env);
    value.Set("done", Napi::Boolean::New(env, message->done));
    value.Set("entries", entries);
    jsCallback.Call({ OK(env, value) });
  } catch (...) {
  }

#ifdef DEBUG
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  TRACE_PTR(message->worker, "elapsed time: %lld msec", elapsed);
#endif
}

void
GetEntriesWorker::InvokeCallbackERR(Napi::Env env,
                                    Napi::Function jsCallback,
                                    GetEntriesWorker* self)
{
  TRACE_PTR(self, "GetEntriesWorker::InvokeCallbackERR");
  try {
    jsCallback.Call({ ERR(env, "Task was canceled") });
  } catch (...) {
  }
}

} // namespace n7zip
