#pragma once

#include "../common.h"
#include <uv.h>

#ifdef _WIN32
  #include <io.h>
#else
  #include <unistd.h>
#endif

namespace n7zip {

class FdInStream
  : public IInStream
  , public CMyUnknownImp
{
public:
  FdInStream(uv_file fd, bool autoclose);
  virtual ~FdInStream();

  MY_UNKNOWN_IMP1(IInStream)

  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);
  STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize);

  static result<IInStream> New(uv_file fd, bool autoclose);
  static result<IInStream> New(const char* path);

private:
  uv_file m_fd;
  bool m_AutoClose = false;
#ifdef _WIN32
  HANDLE m_handle;
#endif
};

} // namespace n7zip
