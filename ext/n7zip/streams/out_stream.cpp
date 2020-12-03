#include "out_stream.h"
#include "../reader/extract_worker.h"

namespace n7zip {

OutBufferData::OutBufferData(UInt32 index, UInt64 offset)
  : m_index(index)
  , m_offset(offset)
{
  TRACE_THIS("+ OutBufferData");
}

OutBufferData::~OutBufferData()
{
  TRACE_THIS("- OutBufferData");
}

Napi::Buffer<char>
OutBufferData::createBuffer(Napi::Env env)
{
  if (m_length == 0) {
    return Napi::Buffer<char>::New(env, 0);
  }

  std::unique_ptr<char[]> buf(new char[m_length]);
  TRACE_PTR(buf.get(), "+ char[]");

  UInt64 start = 0;
  for (auto& str : m_buffers) {
    std::memcpy(buf.get() + start, str.data(), str.size());
    start += str.size();
  }

  return Napi::Buffer<char>::New(env, buf.release(), m_length, [](Napi::Env, char* buf) {
    TRACE_PTR(buf, "- char[]");
    delete[] buf;
  });
}

Napi::Value
OutBufferData::createResult(Napi::Env env)
{
  auto response = Napi::Object::New(env);
  response.Set("index", Napi::Number::New(env, m_index));
  response.Set("offset", Napi::Number::New(env, m_offset));
  response.Set("done", Napi::Boolean::New(env, m_done));
  response.Set("buffer", createBuffer(env));

  return OK(env, response);
}

void
OutBufferData::append(const char* data, UInt32 size)
{
  m_buffers.emplace_back(data, size);
  m_length += size;
}

void
OutBufferData::set_done()
{
  m_done = true;
}

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
      m_worker->postBuffer(std::move(purge()));
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
OutStream::set_done()
{
  m_buffer->set_done();
}

} // namespace n7zip
