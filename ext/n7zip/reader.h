#pragma once

#include "common.h"
#include "n7zip/streams/fd_in_stream.h"

namespace n7zip {

struct StreamData
{
  StreamData(std::unique_ptr<UString> arg_name, CMyComPtr<IInStream> arg_stream)
    : name(std::move(arg_name))
    , stream(arg_stream)
  {}
  std::unique_ptr<UString> name;
  CMyComPtr<IInStream> stream;
};

class Reader
{
public:
  Reader(int fmt_index,
         CMyComPtr<IInArchive> archive,
         CMyComPtr<IArchiveOpenCallback> open_callback,
         std::unique_ptr<std::vector<StreamData>> streams)
    : m_fmt_index(fmt_index)
    , m_archive(archive)
    , m_open_callback(open_callback)
    , m_streams(std::move(streams))
  {
    TRACE("+ Reader %p", this);
  }
  ~Reader() { TRACE("- Reader %p", this); }
  int m_fmt_index;
  CMyComPtr<IInArchive> m_archive;
  CMyComPtr<IArchiveOpenCallback> m_open_callback;
  std::unique_ptr<std::vector<StreamData>> m_streams;
  Napi::ObjectReference m_wrap;
};

Napi::Object
InitReader(Napi::Env env, Napi::Object exports);

} // namespace n7zip
