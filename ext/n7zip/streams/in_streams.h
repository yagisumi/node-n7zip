#pragma once

#include "n7zip/common.h"
#include "fd_in_stream.h"
#include "buffer_in_stream.h"
#include "multi_in_stream.h"

namespace n7zip {

class InStreams
{
  struct InStreamData
  {
    InStreamData(std::unique_ptr<UString>&& _name, CMyComPtr<IInStream>& _stream)
      : name(std::move(_name))
      , stream(_stream)
    {}
    std::unique_ptr<UString> name;
    CMyComPtr<IInStream> stream;
  };

  std::unique_ptr<UString> m_base_dir;
  std::vector<InStreamData> m_streams;

public:
  InStreams(std::unique_ptr<UString>&& base_dir);
  ~InStreams();
  bool append(std::unique_ptr<UString>&& name, CMyComPtr<IInStream>& stream);
  bool append_streams(Napi::Array ary);
  CMyComPtr<IInStream> get_stream(const wchar_t* name);

private:
  IInStream* load_stream(const wchar_t* name);
};

FdInStream*
createFdInStream(Napi::Object arg);

FdInStream*
createFdInStreamFromPath(Napi::Object arg);

BufferInStream*
createBufferInStream(Napi::Object arg);

MultiInStream*
createMultiInStream(Napi::Object arg);

} // namespace n7zip
