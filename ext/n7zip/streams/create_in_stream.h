#pragma once

#include "n7zip/common.h"

#include "fd_in_stream.h"
#include "buffer_in_stream.h"
#include "multi_in_stream.h"

namespace n7zip {

class FdInStream;
class BufferInStream;
class MultiInStream;

FdInStream*
createFdInStream(Napi::Object arg);

FdInStream*
createFdInStreamFromPath(Napi::Object arg);

BufferInStream*
createBufferInStream(Napi::Object arg);

MultiInStream*
createMultiInStream(Napi::Object arg);

} // namespace n7zip
