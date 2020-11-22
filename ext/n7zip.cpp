#include "n7zip/common.h"
#include "n7zip/library.h"
#include "n7zip/canceler.h"
#include "n7zip/reader/reader.h"
#ifdef DEBUG
  #include "n7zip/tester/index.h"
#endif

Napi::Object
Init(Napi::Env env, Napi::Object exports)
{
  TRACE("Init");
#ifdef DEBUG
  exports.Set("DEBUG", Napi::Boolean::New(env, true));

  n7zip::InitTester(env, exports);
#else
  exports.Set("DEBUG", Napi::Boolean::New(env, false));
#endif
  exports.Set("ARCH", Napi::Number::New(env, sizeof(void*) * 8));

  n7zip::InitLibrary(env, exports);
  n7zip::InitReader(env, exports);
  n7zip::Canceler::Init(env, exports);

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
