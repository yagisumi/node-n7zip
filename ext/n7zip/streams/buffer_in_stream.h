#pragma once

#include "../common.h"

namespace n7zip {

class BufferInStream
  : public IInStream
  , public CMyUnknownImp
{
  Napi::Reference<Napi::Buffer<char>> m_ref;
  const char* m_buffer;
  UInt64 m_length;
  UInt64 m_position = 0;
  bool m_ShareBuffer = false;

public:
  static result<IInStream> New(const char* buffer, size_t length);
  static result<IInStream> New(Napi::Reference<Napi::Buffer<char>>&& ref);

  BufferInStream(Napi::Buffer<char> buf, bool ShareBuffer);
  BufferInStream(const char* buffer, size_t length);
  BufferInStream(Napi::Reference<Napi::Buffer<char>>&& ref);
  virtual ~BufferInStream();

  MY_UNKNOWN_IMP1(IInStream)

  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);
  STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize);
};

BufferInStream*
createBufferInStream(Napi::Object arg);

} // namespace n7zip
