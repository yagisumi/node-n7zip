#ifdef DEBUG

  #include "index.h"
  #include "shared_locker.h"
  #include "in_stream_wrap.h"
  #include "n7zip_inspector.h"

namespace n7zip {

Napi::Object
InitTester(Napi::Env env, Napi::Object exports)
{
  auto tester = Napi::Object::New(env);
  exports.Set("tester", tester);

  InitSharedLocker(env, tester);
  InitInStreamWrap(env, tester);
  InitN7zipInspector(env, tester);

  return exports;
}

} // namespace n7zip

#endif
