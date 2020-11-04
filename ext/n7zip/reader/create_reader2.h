#pragma once

#include "../common.h"
#include "create_reader_args.h"

namespace n7zip {

Napi::Object
InitReader2(Napi::Env env, Napi::Object exports);

} // namespace n7zip
