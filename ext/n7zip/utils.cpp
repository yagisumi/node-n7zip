#include "utils.h"

namespace n7zip {

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
