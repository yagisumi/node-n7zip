#pragma once

#include "common.h"
#include "n7zip/callbacks/open_callback.h"

namespace n7zip {

class Reader
{
public:
  Reader(int fmt_index,
         CMyComPtr<IInArchive>& archive,
         CMyComPtr<IArchiveOpenCallback>& open_callback)
    : m_fmt_index(fmt_index)
    , m_archive(archive)
    , m_open_callback(open_callback)
  {
    TRACE("+ Reader %p", this);
  }
  ~Reader() { TRACE("- Reader %p", this); }
  int m_fmt_index;
  CMyComPtr<IInArchive> m_archive;
  CMyComPtr<IArchiveOpenCallback> m_open_callback;
  Napi::ObjectReference m_wrap;
};

Napi::Object
InitReader(Napi::Env env, Napi::Object exports);

} // namespace n7zip
