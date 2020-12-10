#include "get_entries_worker.h"
#include <algorithm>
#include <chrono>

namespace n7zip {

GetEntriesWorker::GetEntriesWorker(Napi::Env env,
                                   Napi::Function callback,
                                   Reader* reader,
                                   GetEntriesArgs&& args)
  : m_reader(reader)
  , m_args(std::move(args))
{
  TRACE_THIS("+ GetEntriesWorker");
  auto n = m_reader->Ref();
  TRACE_THIS("m_reader->Ref(): %u", n);

  if (m_args.canceler) {
    m_args.canceler->Ref();
  }

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

  if (m_args.canceler) {
    m_args.canceler->Unref();
  }

  m_thread.join();
}

void
GetEntriesWorker::execute()
{
  TRACE_THIS("[GetEntriesWorker::execute]");
  auto lock = m_reader->lock();

  for (UInt32 i = m_args.start; i < m_args.end; i += m_args.limit) {
    if (m_args.canceler && m_args.canceler->is_canceled()) {
      auto r_status = m_tsfn.BlockingCall([this](Napi::Env env, Napi::Function jsCallback) {
        postTypeOnly(env, jsCallback, "abort");
      });
      TRACE_THIS("napi_status: %d", r_status);
      break;
    }

    auto end = std::min(i + m_args.limit, m_args.end);
    TRACE_THIS("[GetEntriesWorker::execute] i: %u, end: %u", i, end);

    auto done = end == m_args.end;
    auto response_ptr = new EntriesResponse(m_reader->get_entries(i, end, m_args.prop_ids), done);

    auto r_status = m_tsfn.BlockingCall(
      response_ptr,
      [this](Napi::Env env, Napi::Function jsCallback, EntriesResponse* response_ptr) {
        postEntries(env, jsCallback, response_ptr);
      });
    TRACE_THIS("napi_status: %d", r_status);

    if (r_status != napi_ok) {
      delete response_ptr;
      auto r_err_status = m_tsfn.BlockingCall([this](Napi::Env env, Napi::Function jsCallback) {
        postTypeOnly(env, jsCallback, "error");
      });
      TRACE_THIS("napi_status: %d", r_err_status);
      break;
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
GetEntriesWorker::postTypeOnly(Napi::Env env, Napi::Function jsCallback, const char* type_name)
{
  TRACE_THIS("[GetEntriesWorker::postTypeOnly]");
  try {
    auto res = Napi::Object::New(env);
    res.Set("type", Napi::String::New(env, type_name));
    jsCallback.Call({ res });
  } catch (...) {
  }
}

void
GetEntriesWorker::postEntries(Napi::Env env,
                              Napi::Function jsCallback,
                              EntriesResponse* response_ptr)
{
  std::unique_ptr<EntriesResponse> response(response_ptr);
  TRACE_THIS("[GetEntriesWorker::postEntries]");
#ifdef DEBUG
  auto start = std::chrono::system_clock::now();
#endif

  try {
    auto length = response->entries.size();
    auto entries = Napi::Array::New(env, length);
    for (size_t i = 0; i < length; i++) {
      auto& entry = response->entries[i];
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

    auto res = Napi::Object::New(env);
    res.Set("type", Napi::String::New(env, "entries"));
    res.Set("done", Napi::Boolean::New(env, response->done));
    res.Set("entries", entries);
    jsCallback.Call({ res });
  } catch (...) {
  }

#ifdef DEBUG
  auto end = std::chrono::system_clock::now();
  auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
  TRACE_THIS("elapsed time: %lld msec", elapsed);
#endif
}

} // namespace n7zip
