#pragma once

#include "../common.h"

namespace n7zip {

class OutBufferData
{
  UInt32 m_index;
  UInt64 m_offset;
  UInt32 m_length = 0;
  std::vector<std::string> m_buffers;
  bool m_done = false;
  Int32 m_operation_result = NArchive::NExtract::NOperationResult::kOK;

public:
  OutBufferData(UInt32 index, UInt64 offset);
  ~OutBufferData();
  Napi::Value createResult(Napi::Env env);
  void append(const char* data, UInt32 size);
  void set_done(Int32 result);

private:
  Napi::Buffer<char> createBuffer(Napi::Env env);
};

} // namespace n7zip
