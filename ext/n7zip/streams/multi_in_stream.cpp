#include "multi_in_stream.h"

namespace n7zip {

MultiInStream::MultiInStream(std::unique_ptr<std::vector<CMyComPtr<IInStream>>>&& streams)
  : m_streams(std::move(streams))
{
  TRACE_THIS("+ MultiInStream");

  UInt64 offset = 0;
  m_ranges.reserve(m_streams->size());

  for (const auto& stream : *m_streams) {
    UInt64 len;
    auto r = stream->Seek(0, STREAM_SEEK_END, &len);
    if (r != S_OK) {
      m_is_invalid = true;
      return;
    }
    m_ranges.emplace_back(offset, offset + len);
    offset += len;
    stream->Seek(0, STREAM_SEEK_SET, nullptr);
  }

  m_length = offset;
}

MultiInStream::~MultiInStream()
{
  TRACE_THIS("- MultiInStream");
}

result<IInStream>
MultiInStream::New(std::unique_ptr<std::vector<std::unique_ptr<InStreamArg>>>&& streams)
{
  auto com_streams = std::make_unique<std::vector<CMyComPtr<IInStream>>>();

  for (auto& stream : *(streams.get())) {
    auto r = stream->createInStream();
    if (r.err()) {
      return r;
    } else if (!r.ok()) {
      return err<IInStream>("Unexpected error");
    }
    CMyComPtr<IInStream> in_stream = r.release_ok();
    com_streams->push_back(in_stream);
  }

  if (com_streams->size() == 0) {
    return err<IInStream>("No valid stream");
  } else {
    return ok<IInStream>(new MultiInStream(std::move(com_streams)));
  }
}

STDMETHODIMP
MultiInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
  TRACE_THIS("[MultiInStream::Seek] offset: %lld, seekOrigin: %u (%llu/%llu)",
             offset,
             seekOrigin,
             m_position,
             m_length);

  if (m_is_invalid) {
    return E_FAIL;
  }

  switch (seekOrigin) {
    case STREAM_SEEK_SET:
      break;
    case STREAM_SEEK_CUR:
      offset += m_position;
      break;
    case STREAM_SEEK_END:
      offset += m_length;
      break;
    default:
      return STG_E_INVALIDFUNCTION;
  }

  if (offset < 0) {
    return HRESULT_WIN32_ERROR_NEGATIVE_SEEK;
  }

  m_position = offset;

  if (newPosition) {
    *newPosition = offset;
  }

  return S_OK;
}

STDMETHODIMP
MultiInStream::Read(void* data, UInt32 size, UInt32* processedSize)
{
  TRACE_THIS("[MultiInStream::Read] size: %u (%llu/%llu)", size, m_position, m_length);

  if (m_is_invalid) {
    return E_FAIL;
  }

  if (processedSize) {
    *processedSize = 0;
  }

  if (size == 0) {
    return S_OK;
  }

  if (m_position >= m_length) {
    return S_OK;
  }

  {
    UInt64 rem = m_length - m_position;
    if (size > rem) {
      size = (UInt32)rem;
    }
  }

  UInt64 offset = 0;
  UInt32 total_processed_size = 0;

  for (size_t i = 0; i < m_ranges.size(); i++) {
    auto& range = m_ranges.at(i);
    TRACE_THIS("m_postion: %llu, i: %llu, %llu..%llu", m_position, i, range.begin, range.end);
    if ((range.begin <= m_position) && (m_position < range.end)) {
      UInt32 read_size = range.end - m_position;
      if (size < read_size) {
        read_size = size;
      }

      TRACE_THIS("i: %llu, offset: %llu, size: %u, read_size: %u", i, offset, size, read_size);

      UInt32 processed_size = 0;
      auto& stream = m_streams->at(i);

      auto r_seek = stream->Seek(m_position - range.begin, STREAM_SEEK_SET, nullptr);
      if (r_seek != S_OK) {
        m_is_invalid = true;
        return E_FAIL;
      }

      auto r_read = stream->Read((char*)data + offset, read_size, &processed_size);
      TRACE_THIS("read_size: %u, processed_size: %u", read_size, processed_size);
      if (r_read != S_OK || read_size != processed_size) {
        m_is_invalid = true;
        return E_FAIL;
      }

      total_processed_size += processed_size;
      size -= read_size;
      m_position += read_size;
      offset += read_size;
    }

    if (size <= 0) {
      break;
    }
  }

  if (processedSize) {
    *processedSize = total_processed_size;
  }

  return S_OK;
}

} // namespace n7zip
