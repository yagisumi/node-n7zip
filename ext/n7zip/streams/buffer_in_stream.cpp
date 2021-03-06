#include "buffer_in_stream.h"

namespace n7zip {

BufferInStream::BufferInStream(Napi::Buffer<char> buf, bool ShareBuffer)
{
  TRACE_THIS("+ BufferInStream");

  m_ShareBuffer = ShareBuffer;
  m_length = buf.Length();

  if (m_ShareBuffer) {
    m_ref = Napi::Persistent(buf);
    m_buffer = buf.Data();
  } else {
    m_buffer = new char[m_length];
    std::memcpy((void*)m_buffer, buf.Data(), m_length);
  }
}

BufferInStream::BufferInStream(Napi::Reference<Napi::Buffer<char>>&& ref)
{
  TRACE_THIS("+ BufferInStream");
  m_ShareBuffer = true;
  auto buf = ref.Value();
  m_length = buf.Length();
  m_buffer = buf.Data();
  m_ref = std::move(ref);
}

BufferInStream::BufferInStream(const char* buffer, size_t length)
{
  TRACE_THIS("+ BufferInStream");
  m_ShareBuffer = false;
  m_length = length;
  m_buffer = buffer;
}

BufferInStream::~BufferInStream()
{
  TRACE_THIS("- BufferInStream");

  if (m_ShareBuffer) {
    m_buffer = nullptr;
    m_ref.Unref();
  } else {
    TRACE_PTR(m_buffer, "- buffer");
    delete[] m_buffer;
  }
}

result<IInStream>
BufferInStream::New(const char* buffer, size_t length)
{
  if (buffer == nullptr) {
    return err<IInStream>("Invalid buffer");
  }

  return ok<IInStream>(new BufferInStream(buffer, length));
}

result<IInStream>
BufferInStream::New(Napi::Reference<Napi::Buffer<char>>&& ref)
{
  if (ref.IsEmpty()) {
    return err<IInStream>("Invalid buffer reference");
  }

  return ok<IInStream>(new BufferInStream(std::move(ref)));
}

STDMETHODIMP
BufferInStream::Seek(Int64 offset, UInt32 seekOrigin, UInt64* newPosition)
{
  TRACE_THIS("[BufferInStream::Seek] offset: %lld, seekOrigin: %u (%llu/%llu)",
             offset,
             seekOrigin,
             m_position,
             m_length);

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
BufferInStream::Read(void* data, UInt32 size, UInt32* processedSize)
{
  TRACE_THIS("[BufferInStream::Read] size: %u (%llu/%llu)", size, m_position, m_length);

  if (processedSize) {
    *processedSize = 0;
  }

  if (size == 0) {
    return S_OK;
  }

  if (m_position >= m_length) {
    return S_OK;
  }

  UInt64 rem = m_length - m_position;
  if (size > rem) {
    size = (UInt32)rem;
  }

  std::memcpy(data, m_buffer + m_position, size);
  m_position += size;

  if (processedSize) {
    *processedSize = size;
  }

  return S_OK;
}

} // namespace n7zip
