#include "extract_callback.h"
#include "../streams/out_stream.h"
#include "../reader/extract_worker.h"

namespace n7zip {

ExtractCallback::ExtractCallback(ExtractWorker* worker, UInt32 limit)
  : m_worker(worker)
  , m_limit(limit)
{
  TRACE_THIS("+ ExtractCallback");
}

ExtractCallback::~ExtractCallback()
{
  TRACE_THIS("- ExtractCallback");
}

STDMETHODIMP
ExtractCallback::SetTotal(UInt64 size)
{
  TRACE_THIS("[ExtractCallback::SetTotal] size: %llu", size);
  return S_OK;
}

STDMETHODIMP
ExtractCallback::SetCompleted(const UInt64* completeValue)
{
#ifdef DEBUG
  if (completeValue) {
    TRACE_THIS("[ExtractCallback::SetCompleted] completeValue: %llu", *completeValue);
  } else {
    TRACE_THIS("[ExtractCallback::SetCompleted] completeValue: -");
  }
#endif
  if (m_worker->is_canceled()) {
    return E_ABORT;
  }

  return S_OK;
}

STDMETHODIMP
ExtractCallback::GetStream(UInt32 index, ISequentialOutStream** outStream, Int32 askExtractMode)
{
  TRACE_THIS("[ExtractCallback::GetStream] index: %u, askExtractMode: %d", index, askExtractMode);
  *outStream = nullptr;

  if (askExtractMode != NArchive::NExtract::NAskMode::kExtract) {
    return S_OK;
  }

  // CMyComPtr<OutStream> out_stream(new OutStream(index, m_worker, m_limit));
  m_current_index = index;
  m_current_stream = new OutStream(index, m_worker, m_limit);
  m_current_stream->AddRef();
  *outStream = m_current_stream;

  return S_OK;
}

STDMETHODIMP
ExtractCallback::PrepareOperation(Int32 askExtractMode)
{
  TRACE_THIS("[ExtractCallback::PrepareOperation] askExtractMode: %d", askExtractMode);
  return S_OK;
}

STDMETHODIMP
ExtractCallback::SetOperationResult(Int32 result)
{
  TRACE_THIS("[ExtractCallback::SetOperationResult] result: %d", result);

  if (m_current_stream) {
    m_current_stream->set_done(result);
    m_worker->post_buffer(m_current_stream->purge());
  }

  if (result != NArchive::NExtract::NOperationResult::kOK) {
    //
  }
  return S_OK;
}

} // namespace n7zip
