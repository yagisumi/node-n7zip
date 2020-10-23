#pragma once

#include "common.h"

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

namespace n7zip {

template<typename... Args>
std::string
format(const std::string& fmt, Args... args);

std::string
GuidToString(const GUID* guid);

inline Napi::Object
OK(Napi::Env env)
{
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, true);
  return obj;
}

inline Napi::Object
OK(Napi::Env env, Napi::Value value)
{
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, true);
  obj["value"] = value;
  return obj;
}

inline Napi::Object
ERR(Napi::Env env, Napi::Error error)
{
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, false);
  obj["error"] = error.Value();
  return obj;
}

enum class ErrorType
{
  Error,
  TypeError,
};

inline Napi::Object
ERR(Napi::Env env, const char* message, ErrorType type = ErrorType::Error)
{
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, false);
  if (type == ErrorType::TypeError) {
    obj["error"] = Napi::TypeError::New(env, message).Value();
  } else {
    obj["error"] = Napi::Error::New(env, message).Value();
  }
  return obj;
}

Napi::String
ConvertBStrToNapiString(Napi::Env env, BSTR bstr);

std::unique_ptr<UString>
ConvertNapiStringToUString(Napi::String str);

} // namespace n7zip

inline bool
operator<(REFGUID g1, REFGUID g2)
{
  for (size_t i = 0; i < (sizeof(g1) / sizeof(int)); i++)
    if (((unsigned int*)&g1)[i] != ((unsigned int*)&g2)[i])
      return ((unsigned int*)&g1)[i] < ((unsigned int*)&g2)[i];
  return false;
}
