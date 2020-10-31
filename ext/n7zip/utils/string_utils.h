#pragma once

#include "../base.h"
#include <cstdarg>
#include <cstdio>

namespace n7zip {

// template<typename... Args>
// std::string
// format(const char* fmt, Args const&... args);

std::string
format(const char* fmt, ...);

// template<typename... Args>
// inline std::string
// format(const char* fmt, Args const&... args)
// {
//   return format_impl(fmt, args...);
// }

std::string
GuidToString(const GUID* guid);

Napi::String
ConvertBStrToNapiString(Napi::Env env, BSTR bstr);

std::unique_ptr<UString>
ConvertNapiStringToUString(Napi::String str);

} // namespace n7zip
