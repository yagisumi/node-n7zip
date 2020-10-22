#ifdef DEBUG

  #include "utils.h"

namespace n7zip {

Napi::Value // Buffer | undefined
inspectUString(const Napi::CallbackInfo& info)
{
  auto env = info.Env();

  if (info.Length() == 0 || !(info[0].IsString())) {
    return env.Undefined();
  }

  auto str = info[0].ToString().Utf8Value();
  AString astr;
  astr.SetFrom(str.c_str(), str.length());
  UString ustr;
  ConvertUTF8ToUnicode(astr, ustr);
  return Napi::Buffer<char>::Copy(env, (char*)ustr.Ptr(), ustr.Len() * sizeof(wchar_t));
}

Napi::Object
InitTesterUtils(Napi::Env env, Napi::Object tester)
{
  tester.Set("inspectUString", Napi::Function::New(env, inspectUString));

  return tester;
}

} // namespace n7zip

#endif
