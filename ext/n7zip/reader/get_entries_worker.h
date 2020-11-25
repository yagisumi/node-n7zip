#pragma once

#include "../common.h"
#include "reader.h"
#include "../canceler.h"

namespace n7zip {

struct GetEntriesWorkerArgs
{
  UInt32 limit;
  UInt32 start;
  UInt32 end;
  std::unique_ptr<std::vector<PROPID>> prop_ids;

  GetEntriesWorkerArgs(UInt32 limit,
                       UInt32 start,
                       UInt32 end,
                       std::unique_ptr<std::vector<PROPID>>&& prop_ids)
    : limit(limit)
    , start(start)
    , end(end)
    , prop_ids(std::move(prop_ids))
  {
    TRACE_THIS("+ GetEntriesWorkerArgs");
  }
  ~GetEntriesWorkerArgs() { TRACE_THIS("- GetEntriesWorkerArgs"); }

  GetEntriesWorkerArgs(GetEntriesWorkerArgs&& other)
  {
    TRACE_THIS("+ GetEntriesWorkerArgs");
    this->limit = other.limit;
    this->start = other.start;
    this->end = other.end;
    this->prop_ids = std::move(other.prop_ids);
  };
  GetEntriesWorkerArgs& operator=(GetEntriesWorkerArgs&& rhs) = default;
};

class GetEntriesWorker;

struct GetEntriesMessage
{
  std::vector<Entry> entries;
  bool done = false;
  GetEntriesWorker* worker;

  GetEntriesMessage(std::vector<Entry>&& entries, bool done, GetEntriesWorker* worker)
    : entries(std::move(entries))
    , done(done)
    , worker(worker)
  {
    TRACE_THIS("+ GetEntriesMessage");
  }
  ~GetEntriesMessage() { TRACE_THIS("- GetEntriesMessage"); }
};

class GetEntriesWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  Reader* m_reader;
  GetEntriesWorkerArgs m_args;
  std::unique_ptr<std::vector<PROPID>> m_prop_ids;
  Canceler* m_canceler;

public:
  GetEntriesWorker(Napi::Env env,
                   Napi::Function callback,
                   Reader* reader,
                   GetEntriesWorkerArgs&& args,
                   Canceler* canceler);
  ~GetEntriesWorker();

  void execute();

  static void Finalize(Napi::Env, void*, GetEntriesWorker* self);
  static void InvokeCallbackOK(Napi::Env env,
                               Napi::Function jsCallback,
                               GetEntriesMessage* message_ptr);
  static void GetEntriesWorker::InvokeCallbackERR(Napi::Env env,
                                                  Napi::Function jsCallback,
                                                  GetEntriesWorker* message_ptr);
};

} // namespace n7zip
