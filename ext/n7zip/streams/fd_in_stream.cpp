#include "fd_in_stream.h"

namespace n7zip {

FdInStream::FdInStream(uv_file fd, bool autoclose)
{
  TRACE("+ FdInStream %p", this);
  m_fd = fd;
  m_AutoClose = autoclose;
#ifdef _WIN32
  m_handle = reinterpret_cast<HANDLE>(uv_get_osfhandle(fd));
#endif
}

FdInStream::~FdInStream()
{
  TRACE("- FdInStream %p", this);
  if (m_AutoClose) {
    TRACE("[FdInStream::~FdInStream] autoclose");
    uv_fs_t req;
    uv_fs_close(nullptr, &req, m_fd, nullptr);
    uv_fs_req_cleanup(&req);
  }
}

result<IInStream>
FdInStream::New(uv_file fd, bool AutoClose)
{
  uv_fs_t req;
  uv_buf_t buf = uv_buf_init(nullptr, 0);

  auto r_read = uv_fs_read(nullptr, &req, fd, &buf, 1, 0, nullptr);
  uv_fs_req_cleanup(&req);

  if (r_read < 0) {
    return err<IInStream>(format("Cannot read (file descriptor: %d)", fd));
  } else {
    // auto stream = new FdInStream(fd, AutoClose);
    auto stream = std::make_unique<FdInStream>(fd, AutoClose);
    UInt64 pos;
    auto r_seek = stream->Seek(0, STREAM_SEEK_SET, &pos);
    if (r_seek == S_OK) {
      return ok<IInStream>(std::move(stream));
    } else {
      return err<IInStream>(format("Cannot seek (file descriptor: %d)", fd));
    }
  }
}

result<IInStream>
FdInStream::New(const char* path)
{
  if (path == nullptr) {
    return err<IInStream>("Invalid path");
  }

  uv_fs_t open_req;
  auto r = uv_fs_open(nullptr, &open_req, path, UV_FS_O_RDONLY, 0666, nullptr);
  uv_fs_req_cleanup(&open_req);

  if (r < 0) {
    return err<IInStream>(format("Cannot open file: %s", path));
  }

  return FdInStream::New(r, true);
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
    if (newPosition) {
      *newPosition = r;
    }
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
createFdInStream(Napi::Object arg)
{
  auto fd = arg.Get("source").ToNumber();
  auto AutoClose = true;
  auto auto_close = arg.Get("AutoClose");
  if (auto_close.IsBoolean()) {
    AutoClose = auto_close.ToBoolean().Value();
  }

  return new FdInStream(fd, AutoClose);
}

FdInStream*
createFdInStreamFromPath(Napi::Object arg)
{
  auto path = arg.Get("source").ToString().Utf8Value();
  auto r = FdInStream::New(path.c_str());
  if (r.ok()) {
    return dynamic_cast<FdInStream*>(r.release_ok());
  } else {
    return nullptr;
  }
}

} // namespace n7zip
