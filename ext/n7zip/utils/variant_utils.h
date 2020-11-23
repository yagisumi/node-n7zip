#pragma once

#include "../base.h"

namespace n7zip {

Napi::Value
ConvertPropVariant(Napi::Env env, NWindows::NCOM::CPropVariant& prop);

}
