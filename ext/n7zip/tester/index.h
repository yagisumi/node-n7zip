#pragma once

#ifdef DEBUG

  #include "../common.h"

namespace n7zip {

Napi::Object
InitTester(Napi::Env env, Napi::Object exports);

} // namespace n7zip

#endif
