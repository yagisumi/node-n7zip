#pragma once

#include "../common.h"
#include "reader.h"
#include "reader_wrap.h"

namespace n7zip {

Napi::Object
InitReader(Napi::Env env, Napi::Object exports);

} // namespace n7zip
