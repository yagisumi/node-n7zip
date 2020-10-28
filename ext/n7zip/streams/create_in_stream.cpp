#include "create_in_stream.h"

namespace n7zip {

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
  auto AutoClose = true;

  uv_fs_t open_req;
  auto r = uv_fs_open(nullptr, &open_req, path.c_str(), UV_FS_O_RDONLY, 0666, nullptr);
  if (r < 0) {
    return nullptr;
  }

  return new FdInStream(r, AutoClose);
}

BufferInStream*
createBufferInStream(Napi::Object arg)
{
  auto buf = arg.Get("source").As<Napi::Buffer<char>>();
  auto ShareBuffer = false;
  auto share_buffer = arg.Get("ShareBuffer");
  if (share_buffer.IsBoolean()) {
    ShareBuffer = share_buffer.ToBoolean().Value();
  }

  return new BufferInStream(buf, ShareBuffer);
}

MultiInStream*
createMultiInStream(Napi::Object arg)
{
  auto streams = std::make_unique<std::vector<CMyComPtr<IInStream>>>();
  auto ary = arg.Get("source").As<Napi::Array>();
  for (uint32_t i = 0; i < ary.Length(); i++) {
    auto v = ary.Get(i);
    if (v.IsObject()) {
      auto obj = v.ToObject();
      auto source = obj.Get("source");
      CMyComPtr<IInStream> stream;
      if (source.IsNumber()) {
        stream = createFdInStream(obj);
      } else if (source.IsString()) {
        stream = createFdInStreamFromPath(obj);
      } else if (source.IsBuffer()) {
        stream = createBufferInStream(obj);
      } else {
        TRACE("[createMultiInStream] unexpected source type: %u", i);
        return nullptr;
      }

      if (stream) {
        streams->push_back(stream);
      } else {
        TRACE("[createMultiInStream] failed to create stream: %u", i);
        return nullptr;
      }
    }
  }

  if (streams->size() > 0) {
    return new MultiInStream(std::move(streams));
  } else {
    TRACE("[createMultiInStream] streams is empty");
    return nullptr;
  }
}

} // namespace n7zip
