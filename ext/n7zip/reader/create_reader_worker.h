#pragma once

#include "../common.h"
#include "create_reader_args.h"
#include "reader.h"

namespace n7zip {

struct ReaderArgs
{
  int fmt_index;
  CMyComPtr<IInArchive> archive;
  CMyComPtr<IArchiveOpenCallback> open_callback;
};

class CreateReaderWorker
{
  Napi::ThreadSafeFunction m_tsfn;
  std::thread m_thread;
  std::unique_ptr<CreateReaderArg> m_arg;
  ReaderArgs m_reader_args;
  std::unique_ptr<error> m_err;

public:
  CreateReaderWorker(std::unique_ptr<CreateReaderArg>&& arg,
                     Napi::Env env,
                     Napi::Function callback);
  ~CreateReaderWorker();

  void abort(std::unique_ptr<error>&& err);
  void finish(int fmt_index,
              CMyComPtr<IInArchive>& archive,
              CMyComPtr<IArchiveOpenCallback>& open_callback);
  void execute();

  static void Finalize(Napi::Env, void*, CreateReaderWorker* self);
  static void InvokeCallback(Napi::Env env, Napi::Function jsCallback, CreateReaderWorker* self);
};

} // namespace n7zip
