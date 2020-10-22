#pragma once

#include "common.h"

namespace n7zip {

struct StreamData
{
  std::unique_ptr<UString> name;
  CMyComPtr<IInStream> stream;
};

class Reader
{
  // fmtIndex
  // IInArchive
  // m_wrap
  // streams
  // opencallback
};

} // namespace n7zip
