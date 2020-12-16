#include "out_stream.h"
#include "../reader/extract_worker.h"

namespace n7zip {

OutStream::OutStream(UInt32 index, ExtractWorker* worker, UInt32 limit)
  : m_index(index)
  , m_worker(worker)
{
  TRACE_THIS("+ OutStream");
  if (limit > MIN_LIMIT) {
    m_limit = limit;
  }
  m_buffer = std::make_unique<OutBufferData>(m_index, m_length);
}

OutStream::~OutStream()
{
  TRACE_THIS("- OutStream");
}

STDMETHODIMP
OutStream::Write(const void* data, UInt32 size, UInt32* processedSize)
{
  TRACE_THIS("[OutStream::Write] size: %u", size);
  if (m_buffer) {
    m_buffer->append((const char*)data, size);
    *processedSize = size;
    m_length += size;
    if (m_length >= m_limit) {
      m_worker->post_buffer(std::move(purge()));
      m_buffer = std::make_unique<OutBufferData>(m_index, m_length);
    }
    return S_OK;
  } else {
    *processedSize = 0;
    return E_FAIL;
  }
}

std::unique_ptr<OutBufferData>
OutStream::purge()
{
  return std::move(m_buffer);
}

void
OutStream::set_done(Int32 result)
{
  m_buffer->set_done(result);
}

} // namespace n7zip
