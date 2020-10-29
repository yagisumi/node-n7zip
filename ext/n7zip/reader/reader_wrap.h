#pragma once

#include "../common.h"
#include "reader.h"
#include "close_worker.h"

namespace n7zip {

class ReaderWrap : public Napi::ObjectWrap<ReaderWrap>
{
public:
  std::unique_ptr<Reader> m_reader;

  ReaderWrap(const Napi::CallbackInfo& info);
  ~ReaderWrap();

  static Napi::FunctionReference constructor;
  static Napi::Object Init(Napi::Env env, Napi::Object exports);

  Napi::Value isClosed(const Napi::CallbackInfo& info);
  Napi::Value close(const Napi::CallbackInfo& info);
};

} // namespace n7zip
