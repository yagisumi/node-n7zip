#pragma once

#ifdef DEBUG

  #include "../common.h"

namespace n7zip {

class InStreamWrap : public Napi::ObjectWrap<InStreamWrap>
{
public:
  static Napi::Object Init(Napi::Env env, Napi::Object tester);
  InStreamWrap(const Napi::CallbackInfo& info);
  static Napi::FunctionReference constructor;

  Napi::Value Seek(const Napi::CallbackInfo& info);
  Napi::Value Read(const Napi::CallbackInfo& info);
  CMyComPtr<IInStream> m_inStream;
};

Napi::Object
InitInStreamWrap(Napi::Env env, Napi::Object tester);

} // namespace n7zip

#endif
