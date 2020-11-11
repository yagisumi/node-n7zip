#pragma once

#ifdef DEBUG

  #include "../common.h"

namespace n7zip {

Napi::Object
InitN7zipInspector(Napi::Env env, Napi::Object tester);

} // namespace n7zip

#endif
