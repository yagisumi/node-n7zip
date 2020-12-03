#pragma once

#include "../common.h"
// #include "../reader/extract_worker.h"

namespace n7zip {

class OutBufferData
{
  UInt32 m_index;
  UInt64 m_offset;
  UInt32 m_length = 0;
  std::vector<std::string> m_buffers;
  bool m_done = false;

public:
  OutBufferData(UInt32 index, UInt64 offset);
  ~OutBufferData();
  Napi::Value createResult(Napi::Env env);
  void append(const char* data, UInt32 size);
  void set_done();

private:
  Napi::Buffer<char> createBuffer(Napi::Env env);
};

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
  void set_done();
};

} // namespace n7zip
