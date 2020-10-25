#include "fd_in_stream.h"

namespace n7zip {

FdInStream::FdInStream(uv_file fd, bool autoclose)
{
  TRACE("+ FdInStream %p", this);
  m_fd = fd;
  m_autoclose = autoclose;
#ifdef _WIN32
  m_handle = reinterpret_cast<HANDLE>(uv_get_osfhandle(fd));
#endif
}
FdInStream::~FdInStream()
{
  TRACE("- FdInStream %p", this);
  if (m_autoclose) {
    TRACE("[FdInStream::~FdInStream] autoclose");
    uv_fs_t req;
    uv_fs_close(nullptr, &req, m_fd, nullptr);
    uv_fs_req_cleanup(&req);
  }
}

STDMETHODIMP
FdInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
  TRACE("[FdInStream::Seek] offset: %lld, seekOrigin: %u", offset, seekOrigin);
#ifdef _WIN32
  auto r =
    SetFilePointerEx(m_handle, *(LARGE_INTEGER*)(&offset), (PLARGE_INTEGER)newPosition, seekOrigin);
  TRACE("SetFilePointerEx: %d", r);
  if (r == 0) {
    TRACE("[FdInStream::Read] SeekError");
    return E_ABORT;
  }
#else
  auto r = lseek(m_fd, offset, seekOrigin);
  TRACE("lseek: %lu", r);
  if (r < 0) {
    TRACE("[FdInStream::Read] SeekError");
    return E_ABORT;
  } else {
    *newPosition = r;
  }
#endif

  return S_OK;
}

STDMETHODIMP
FdInStream::Read(void* data, UInt32 size, UInt32* processedSize)
{
  uv_fs_t req;
  uv_buf_t buf = uv_buf_init((char*)data, size);

  auto r = uv_fs_read(nullptr, &req, m_fd, &buf, 1, -1, nullptr);
  uv_fs_req_cleanup(&req);

  if (r < 0) {
    TRACE("[FdInStream::Read] ReadError");
    return E_ABORT;
  }

  *processedSize = r;
  return S_OK;
}

FdInStream*
FdInStream::New(uv_file fd, bool autoclose)
{
  uv_fs_t req;
  uv_buf_t buf = uv_buf_init(nullptr, 0);

  auto r = uv_fs_read(nullptr, &req, fd, &buf, 1, 0, nullptr);
  if (r < 0) {
    return nullptr;
  } else {
    return new FdInStream(fd, autoclose);
  }
}

} // namespace n7zip
