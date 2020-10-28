#include "multi_in_stream.h"

namespace n7zip {

MultiInStream::MultiInStream(std::unique_ptr<std::vector<CMyComPtr<IInStream>>>&& streams)
  : m_streams(std::move(streams))
{
  TRACE("+ MultiInStream %p", this);

  UInt64 offset = 0;
  m_ranges.reserve(m_streams->size());

  for (const auto& stream : *m_streams.get()) {
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
  TRACE("- MultiInStream %p", this);
}

STDMETHODIMP
MultiInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
  TRACE("[MultiInStream::Seek] offset: %lld, seekOrigin: %u (%llu/%llu)",
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
  TRACE("[MultiInStream::Read] size: %u (%llu/%llu)", size, m_position, m_length);

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
    TRACE("m_postion: %llu, i: %llu, %llu..%llu", m_position, i, range.begin, range.end);
    if ((range.begin <= m_position) && (m_position < range.end)) {
      UInt32 read_size = range.end - m_position;
      if (size < read_size) {
        read_size = size;
      }

      TRACE("i: %llu, offset: %llu, size: %u, read_size: %u", i, offset, size, read_size);

      UInt32 processed_size = 0;
      auto& stream = m_streams->at(i);

      auto r_seek = stream->Seek(m_position - range.begin, STREAM_SEEK_SET, nullptr);
      if (r_seek != S_OK) {
        m_is_invalid = true;
        return E_FAIL;
      }

      auto r_read = stream->Read((char*)data + offset, read_size, &processed_size);
      TRACE("read_size: %u, processed_size: %u", read_size, processed_size);
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

MultiInStream*
createMultiInStream(Napi::Object arg)
{
  auto streams = std::make_unique<std::vector<CMyComPtr<IInStream>>>();
  auto ary = arg.Get("source").As<Napi::Array>();
  for (uint32_t i = 0; i < ary.Length(); i++) {
    auto v = ary.Get(i);
    if (v.IsObject()) {
      auto obj = v.ToObject();
      auto source = obj.Get("source");
      CMyComPtr<IInStream> stream;
      if (source.IsNumber()) {
        stream = createFdInStream(obj);
      } else if (source.IsString()) {
        stream = createFdInStreamFromPath(obj);
      } else if (source.IsBuffer()) {
        stream = createBufferInStream(obj);
      } else {
        TRACE("[createMultiInStream] unexpected source type: %u", i);
        return nullptr;
      }

      if (stream) {
        streams->push_back(stream);
      } else {
        TRACE("[createMultiInStream] failed to create stream: %u", i);
        return nullptr;
      }
    }
  }

  if (streams->size() > 0) {
    return new MultiInStream(std::move(streams));
  } else {
    TRACE("[createMultiInStream] streams is empty");
    return nullptr;
  }
}

} // namespace n7zip
