#pragma once

#ifdef DEBUG

  #include "n7zip/common.h"
  #include "shared_locker.h"

namespace n7zip {

Napi::Object
InitTester(Napi::Env env, Napi::Object exports);

} // namespace n7zip

#endif
