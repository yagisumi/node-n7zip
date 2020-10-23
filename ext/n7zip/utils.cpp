#include "utils.h"

namespace n7zip {

template<typename... Args>
std::string
format(const std::string& fmt, Args... args)
{
  size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args...);
  std::string buf;
  buf.reserve(len);
  std::snprintf(&buf[0], len + 1, fmt.c_str(), args...);
  return buf;
}

std::string
GuidToString(const GUID* guid)
{
  return format( //
    "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
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
ConvertBStrToNapiString(Napi::Env env, const BSTR bstr)
{
#ifndef _WIN32
  return Napi::String::New(env, (char16_t*)bstr);
#else
  UString ustr;
  ustr.SetFromBstr(bstr);
  AString astr;
  ConvertUnicodeToUTF8(ustr, astr);
  return Napi::String::New(env, astr.Ptr());
#endif
}

std::unique_ptr<UString>
ConvertNapiStringToUString(Napi::String str)
{
#ifdef _WIN32
  auto ustr = std::make_unique<UString>();
  auto utf16str = str.Utf16Value();
  ustr->SetFrom((wchar_t*)utf16str.c_str(), utf16str.length());
  return ustr;
#else
  auto ustr = std::make_unique<UString>();
  AString astr;
  auto utf8str = str.Utf8Value();
  astr.SetFrom(utf8str.c_str(), utf8str.length());
  ConvertUTF8ToUnicode(astr, *ustr);
  return ustr;
#endif
}

} // namespace n7zip
