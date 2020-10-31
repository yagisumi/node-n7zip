#include "string_utils.h"

namespace n7zip {

std::string
format(const char* fmt, ...)
{
  std::va_list list;
  va_start(list, fmt);
  std::string buf;
  auto len = std::vsnprintf(nullptr, 0, fmt, list);
  if (len > 0) {
    buf.reserve(len);
    std::vsnprintf(&buf[0], len + 1, fmt, list);
  }
  va_end(list);

  return buf;
}

std::string
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
