#pragma once

#include "../common.h"
#include "reader.h"

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
  Napi::Value getNumberOfItems(const Napi::CallbackInfo& info);
  Napi::Value getNumberOfArchiveProperties(const Napi::CallbackInfo& info);
  Napi::Value getNumberOfProperties(const Napi::CallbackInfo& info);
  Napi::Value close(const Napi::CallbackInfo& info);
  Napi::Value getPropertyInfo(const Napi::CallbackInfo& info);
};

} // namespace n7zip
