#pragma once

#include "n7zip/common.h"

namespace n7zip {

class BufferInStream
  : public IInStream
  , public CMyUnknownImp
{
  Napi::Reference<Napi::Buffer<char>> m_ref;
  char* m_buffer;
  UInt64 m_length;
  UInt64 m_position = 0;
  bool m_ShareBuffer = false;

public:
  BufferInStream(Napi::Buffer<char> buf, bool ShareBuffer);
  virtual ~BufferInStream();

  MY_UNKNOWN_IMP1(IInStream)

  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);
  STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize);
};

} // namespace n7zip
