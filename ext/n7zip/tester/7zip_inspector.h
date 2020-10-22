#pragma once

#ifdef DEBUG

  #include "n7zip/common.h"

namespace n7zip {

Napi::Object
Init7zipInspector(Napi::Env env, Napi::Object tester);

} // namespace n7zip

#endif
