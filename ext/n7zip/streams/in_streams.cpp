#include "in_streams.h"

namespace n7zip {

InStreams::InStreams(std::unique_ptr<UString>&& base_dir)
  : m_base_dir(std::move(base_dir))
{
  TRACE("+ InStreams %p", this);
}

InStreams::~InStreams()
{
  TRACE("- InStreams %p", this);
}

bool
InStreams::append_streams(Napi::Array ary)
{
  m_streams.reserve(ary.Length());

  for (uint32_t i = 0; i < ary.Length(); i++) {
    auto v = ary.Get(i);
    if (!v.IsObject()) {
      TRACE("[InStreams::append_streams] unexpected stream data isn't object type: %u", i);
      return false;
    }

    auto obj = v.ToObject();
    auto v_source = obj.Get("source");
    CMyComPtr<IInStream> stream;

    if (v_source.IsArray()) {
      stream = createMultiInStream(obj);
    } else if (v_source.IsNumber()) {
      stream = createFdInStream(obj);
    } else if (v_source.IsString()) {
      stream = createFdInStreamFromPath(obj);
    } else if (v_source.IsBuffer()) {
      stream = createBufferInStream(obj);
    } else {
      TRACE("[InStreams::append_streams] unexpected source type: %u", i);
      return false;
    }

    if (!stream) {
      TRACE("[InStreams::append_streams] failed to create stream: %u", i);
      return false;
    }

    auto stream_name = std::make_unique<UString>();
    AString astr;
    auto v_name = obj.Get("name");
    if (!v_name.IsString()) {
      TRACE("[InStreams::append_streams] unexpected stream name");
      astr = "no-name";
    } else {
      auto name_str = v_name.ToString();
      auto name_u8str = name_str.Utf8Value();
      astr.SetFrom(name_u8str.c_str(), name_u8str.length());
    }
    ConvertUTF8ToUnicode(astr, *stream_name);

    auto r = append(std::move(stream_name), stream);
    if (!r) {
      TRACE("[InStreams::append_streams] failed to append stream data");
      return false;
    }
  }

  return true;
}

bool
InStreams::append(std::unique_ptr<UString>&& name, CMyComPtr<IInStream>& stream)
{
  if (!stream) {
    return false;
  }

  m_streams.emplace_back(std::move(name), stream);
  return true;
}

CMyComPtr<IInStream>
InStreams::get_stream(const wchar_t* name)
{
  CMyComPtr<IInStream> ret;
  for (auto& stream : m_streams) {
    if (stream.name && *stream.name == name) {
      ret = stream.stream;
      return ret;
    }
  }

  ret = load_stream(name);
  if (ret) {
    append(std::make_unique<UString>(name), ret);
  }

  return ret;
}

IInStream*
InStreams::load_stream(const wchar_t* name)
{
  UString ustr;
  if (m_base_dir) {
    ustr += *m_base_dir;
  }
  ustr += name;
  AString astr;
  ConvertUnicodeToUTF8(ustr, astr);

  return FdInStream::New(astr.Ptr());
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
