#pragma once

#include "../common.h"
#include "reader.h"
#include "reader_wrap.h"
#include "create_reader_args.h"

namespace n7zip {

class CreateReaderWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  std::unique_ptr<CreateReaderArg> m_arg;
  std::unique_ptr<Reader> m_reader;
  std::unique_ptr<error> m_err;

public:
  CreateReaderWorker(std::unique_ptr<CreateReaderArg>&& arg, Napi::Env env, Napi::Function func);
  ~CreateReaderWorker();
  void abort(std::unique_ptr<error>&& err);
  void finish(std::unique_ptr<Reader>&& reader);
  void execute();

  static void Finalize(Napi::Env, void*, CreateReaderWorker* self);
  static void Invoke(Napi::Env env, Napi::Function jsCallback, CreateReaderWorker* self);
};

} // namespace n7zip
