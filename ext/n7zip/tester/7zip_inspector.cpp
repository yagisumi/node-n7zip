#ifdef DEBUG

  #include "7zip_inspector.h"

namespace n7zip {

Napi::Value // Buffer | undefined
inspectUString(const Napi::CallbackInfo& info)
{
  auto env = info.Env();

  if (info.Length() == 0 || !(info[0].IsString())) {
    return env.Undefined();
  }

  auto str = info[0].ToString();
  auto ustr = ConvertNapiStringToUString(str);
  return Napi::Buffer<char>::Copy(env, (char*)ustr->Ptr(), ustr->Len() * sizeof(wchar_t));
}

Napi::Object
Init7zipInspector(Napi::Env env, Napi::Object tester)
{
  tester.Set("inspectUString", Napi::Function::New(env, inspectUString));

  return tester;
}

} // namespace n7zip

#endif
