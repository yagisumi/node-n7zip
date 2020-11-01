#pragma once

#include "../base.h"
#include <cstdarg>
#include <cstdio>

namespace n7zip {

std::string
format(const char* fmt, ...);

inline std::string
GuidToString(const GUID* guid)
{
  return format( //
    "%08X-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
    guid->Data1,
    guid->Data2,
    guid->Data3,
    guid->Data4[0],
    guid->Data4[1],
    guid->Data4[2],
    guid->Data4[3],
    guid->Data4[4],
    guid->Data4[5],
    guid->Data4[6],
    guid->Data4[7]);
}

Napi::String
ConvertBStrToNapiString(Napi::Env env, BSTR bstr);

std::unique_ptr<UString>
ConvertNapiStringToUString(Napi::String str);

} // namespace n7zip
