#pragma once

#include "../common.h"
#include "out_buffer_data.h"
// #include "../reader/extract_worker.h"

namespace n7zip {

class ExtractWorker;

class OutStream
  : public ISequentialOutStream
  , public CMyUnknownImp
{
  ExtractWorker* m_worker;
  std::unique_ptr<OutBufferData> m_buffer;
  UInt64 m_length = 0;
  UInt32 m_limit = 1048576;
  UInt32 m_index;

public:
  static const UInt32 MIN_LIMIT = 1048576;
  OutStream(UInt32 index, ExtractWorker* worker, UInt32 limit);
  ~OutStream();

  MY_UNKNOWN_IMP1(ISequentialOutStream)
  STDMETHOD(Write)(const void* data, UInt32 size, UInt32* processedSize);

  std::unique_ptr<OutBufferData> purge();
  void set_done(Int32 result);
};

} // namespace n7zip
