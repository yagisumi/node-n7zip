#include <napi.h>

Napi::Object
Init(Napi::Env env, Napi::Object exports)
{
#ifdef DEBUG
  exports.Set("DEBUG", Napi::Boolean::New(env, true));
#else
  exports.Set("DEBUG", Napi::Boolean::New(env, false));
#endif
  exports.Set("ARCH", Napi::Number::New(env, sizeof(void*) * 8));
  return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, Init)
