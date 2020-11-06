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

std::unique_ptr<error>
InStreams::append_streams(std::unique_ptr<std::vector<std::unique_ptr<InStreamArg>>>&& streams)
{
  TRACE("InStreams::append_streams");
  for (auto& stream : *streams) {
    auto r_stream = stream->createInStream();
    if (r_stream.err()) {
      return r_stream.move_err();
    } else if (!r_stream.ok()) {
      return std::make_unique<error>("Unexpected error");
    }
    CMyComPtr<IInStream> in_stream = r_stream.release_ok();

    auto r_append = append(std::move(stream->name), in_stream);
    if (!r_append) { // never
      return std::make_unique<error>("Unexpected error");
    }
  }

  return std::unique_ptr<error>();
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
InStreams::get_stream_by_name(const wchar_t* name)
{
  TRACE("[InStreams::get_stream_by_name]");
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
