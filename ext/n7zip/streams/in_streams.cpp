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
  TRACE("[InStreams::get_stream]");
  CMyComPtr<IInStream> ret;
  for (auto& stream : m_streams) {
    if (stream.name && *stream.name == name) {
      ret = stream.stream;
      return ret;
    }
  }

  ret = load_stream(name);
  if (ret) {
    TRACE("loaded");
    append(std::make_unique<UString>(name), ret);
  }

  return ret;
}

CMyComPtr<IInStream>
InStreams::get_stream_by_index(size_t index)
{
  TRACE("[InStreams::get_stream_by_index]");
  CMyComPtr<IInStream> stream;

  if (index < m_streams.size()) {
    stream = m_streams[index].stream;
  }

  return stream;
}

const std::unique_ptr<UString>&
InStreams::get_name(size_t index)
{
  static const std::unique_ptr<UString> empty;
  if (index < m_streams.size()) {
    return m_streams[index].name;
  } else {
    return empty;
  }
}

IInStream*
InStreams::load_stream(const wchar_t* name)
{
  TRACE("[InStreams::load_stream]");
  UString ustr;
  if (m_base_dir) {
    ustr += *m_base_dir;
  }
  ustr += name;
  AString astr;
  ConvertUnicodeToUTF8(ustr, astr);

  auto r = FdInStream::New(astr.Ptr());
  if (r.ok()) {
    return r.release_ok();
  } else {
    return nullptr;
  }
}

} // namespace n7zip
