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

struct EntriesResponse
{
  std::vector<Entry> entries;
  bool done = false;

  EntriesResponse(std::vector<Entry>&& entries, bool done)
    : entries(std::move(entries))
    , done(done)
  {
    TRACE_THIS("+ EntriesResponse");
  }
  ~EntriesResponse() { TRACE_THIS("- EntriesResponse"); }
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
  void postTypeOnly(Napi::Env env, Napi::Function jsCallback, const char* type_name);
  void postEntries(Napi::Env env, Napi::Function jsCallback, EntriesResponse* message_ptr);

  static void Finalize(Napi::Env, void*, GetEntriesWorker* self);
};

} // namespace n7zip
