#pragma once

#include "n7zip/common.h"

namespace n7zip {

class MultiInStream
  : public IInStream
  , public CMyUnknownImp
{
  struct Range
  {
    UInt64 begin;
    UInt64 end;
    Range(UInt64 _begin, UInt64 _end)
      : begin(_begin)
      , end(_end)
    {}
  };
  std::unique_ptr<std::vector<CMyComPtr<IInStream>>> m_streams;
  UInt64 m_length;
  UInt64 m_position = 0;
  std::vector<Range> m_ranges;
  bool m_is_invalid = false;

public:
  MultiInStream(std::unique_ptr<std::vector<CMyComPtr<IInStream>>>&& streams);
  virtual ~MultiInStream();

  MY_UNKNOWN_IMP1(IInStream)

  STDMETHOD(Seek)(Int64 offset, UInt32 seekOrigin, UInt64* newPosition);
  STDMETHOD(Read)(void* data, UInt32 size, UInt32* processedSize);
};

} // namespace n7zip
