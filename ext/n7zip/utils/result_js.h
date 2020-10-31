#pragma once

#include "../base.h"
#include "string_utils.h"

namespace n7zip {

inline Napi::Object
OK(Napi::Env env)
{
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, true);
  obj["value"] = env.Undefined();
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

template<typename... Args>
inline Napi::Object
ERR(Napi::Env env, const char* fmt, Args const&... args)
{
  auto message = format(fmt, args...);
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, false);
  obj["error"] = Napi::Error::New(env, message.c_str()).Value();
  return obj;
}

template<typename... Args>
inline Napi::Object
ERR_TE(Napi::Env env, const char* fmt, Args const&... args)
{
  auto message = format(fmt, args...);
  auto obj = Napi::Object::New(env);
  obj["ok"] = Napi::Boolean::New(env, false);
  obj["error"] = Napi::TypeError::New(env, message.c_str()).Value();
  return obj;
}

} // namespace n7zip
