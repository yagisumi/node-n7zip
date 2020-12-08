#pragma once

#include "../common.h"
#include "reader.h"
#include "../canceler.h"

namespace n7zip {

struct GetEntriesArgs
{
  UInt32 limit;
  UInt32 start;
  UInt32 end;
  std::unique_ptr<std::vector<PROPID>> prop_ids;
  Canceler* canceler;

  GetEntriesArgs(UInt32 limit,
                 UInt32 start,
                 UInt32 end,
                 std::unique_ptr<std::vector<PROPID>>&& prop_ids,
                 Canceler* canceler)
    : limit(limit)
    , start(start)
    , end(end)
    , prop_ids(std::move(prop_ids))
    , canceler(canceler)
  {
    TRACE_THIS("+ GetEntriesArgs");
  }
  ~GetEntriesArgs() { TRACE_THIS("- GetEntriesArgs"); }

  GetEntriesArgs(GetEntriesArgs&& other)
  {
    TRACE_THIS("+ GetEntriesArgs");
    this->limit = other.limit;
    this->start = other.start;
    this->end = other.end;
    this->prop_ids = std::move(other.prop_ids);
    this->canceler = other.canceler;
  };
  GetEntriesArgs& operator=(GetEntriesArgs&& rhs) = default;
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
  GetEntriesArgs m_args;

public:
  GetEntriesWorker(Napi::Env env, Napi::Function callback, Reader* reader, GetEntriesArgs&& args);
  ~GetEntriesWorker();

  void execute();

  static void Finalize(Napi::Env, void*, GetEntriesWorker* self);
  static void InvokeCallbackOnOK(Napi::Env env,
                                 Napi::Function jsCallback,
                                 GetEntriesMessage* message_ptr);
  static void InvokeCallbackOnError(Napi::Env env,
                                    Napi::Function jsCallback,
                                    GetEntriesWorker* message_ptr);
};

} // namespace n7zip
