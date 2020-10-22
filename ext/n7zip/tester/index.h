#pragma once

#ifdef DEBUG

  #include "n7zip/common.h"
  #include "shared_locker.h"
  #include "in_stream_wrap.h"
  #include "utils.h"

namespace n7zip {

Napi::Object
InitTester(Napi::Env env, Napi::Object exports);

} // namespace n7zip

#endif
