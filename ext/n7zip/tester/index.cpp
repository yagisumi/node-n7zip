#ifdef DEBUG

  #include "index.h"

namespace n7zip {

Napi::Object
InitTester(Napi::Env env, Napi::Object exports)
{
  auto tester = Napi::Object::New(env);
  exports.Set("tester", tester);

  InitSharedLocker(env, tester);

  return exports;
}

} // namespace n7zip

#endif
