#include "out_buffer_data.h"

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
  response.Set("operationResult", Napi::Number::New(env, m_operation_result));
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
OutBufferData::set_done(Int32 result)
{
  m_done = true;
  m_operation_result = result;
}

} // namespace n7zip
