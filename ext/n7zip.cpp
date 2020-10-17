#include "n7zip/common.h"
#ifdef DEBUG
  #include "n7zip/test_object.h"
#endif

Napi::Object
Init(Napi::Env env, Napi::Object exports)
{
  TRACE("Init");
#ifdef DEBUG
  exports.Set("DEBUG", Napi::Boolean::New(env, true));
  n7zip::InitTestObject(env, exports);
#else
  exports.Set("DEBUG", Napi::Boolean::New(env, false));
#endif
  exports.Set("ARCH", Napi::Number::New(env, sizeof(void*) * 8));

  // exports.Set("loadLibrary", Napi::Function::New(env, n7zip::loadLibrary));
  // exports.Set("getFormats", Napi::Function::New(env, n7zip::getFormats));
  // exports.Set("getCodecs", Napi::Function::New(env, n7zip::getCodecs));
  n7zip::InitLibrary(env, exports);

  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
