#pragma once

#include "../common.h"
// #include "../reader/extract_worker.h"
#include "../streams/out_stream.h"

namespace n7zip {

class ExtractWorker;

class ExtractCallback
  : public IArchiveExtractCallback
  , public CMyUnknownImp
{
  ExtractWorker* m_worker;
  UInt32 m_limit;
  UInt32 m_current_index;
  CMyComPtr<OutStream> m_current_stream;

public:
  ExtractCallback(ExtractWorker* worker, UInt32 limit);
  ~ExtractCallback();

  MY_UNKNOWN_IMP1(IArchiveExtractCallback)

  // IArchiveExtractCallback
  // IProgress
  STDMETHOD(SetTotal)(UInt64 size);
  STDMETHOD(SetCompleted)(const UInt64* completeValue);

  STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode);
  STDMETHOD(PrepareOperation)(Int32 askExtractMode);
  STDMETHOD(SetOperationResult)(Int32 resultOperationResult);
};

} // namespace n7zip
