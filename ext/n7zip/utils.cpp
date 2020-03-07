#include "utils.h"

namespace n7zip {

Napi::String
BStrToNString(Napi::Env env, const BSTR bstr)
{
#ifdef _WIN32
  return Napi::String::New(env, (char16_t*)bstr);
#else
  UString ustr(bstr);
  AString astr;
  ConvertUnicodeToUTF8(ustr, astr);
  return Napi::String::New(env, astr.Ptr());
#endif
}

} // namespace n7zip
