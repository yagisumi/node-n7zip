#pragma once

#include "../common.h"
#include "reader.h"

namespace n7zip {

class GetArchivePropertiesWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  Reader* m_reader;
  std::unique_ptr<std::vector<PROPID>> m_prop_ids;
  std::vector<EntryProperty> m_result;

public:
  GetArchivePropertiesWorker(Napi::Env env,
                             Napi::Function callback,
                             Reader* reader,
                             std::unique_ptr<std::vector<PROPID>>&& prop_ids);
  ~GetArchivePropertiesWorker();

  void execute();

  static void Finalize(Napi::Env, void*, GetArchivePropertiesWorker* self);
  static void InvokeCallback(Napi::Env env,
                             Napi::Function jsCallback,
                             GetArchivePropertiesWorker* self);
};

} // namespace n7zip
