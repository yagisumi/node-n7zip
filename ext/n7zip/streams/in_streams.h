#pragma once

#include "n7zip/common.h"
#include "fd_in_stream.h"
#include "buffer_in_stream.h"
#include "multi_in_stream.h"

namespace n7zip {

struct InStreamData
{
  std::unique_ptr<UString> name;
  CMyComPtr<IInStream> stream;

  InStreamData(std::unique_ptr<UString>&& _name, CMyComPtr<IInStream>& _stream)
    : name(std::move(_name))
    , stream(_stream)
  {}
};

class InStreams
{
  std::unique_ptr<UString> m_base_dir;
  std::vector<InStreamData> m_streams;

public:
  InStreams(std::unique_ptr<UString>&& base_dir);
  ~InStreams();
  bool append(std::unique_ptr<UString>&& name, CMyComPtr<IInStream>& stream);
  bool append_streams(Napi::Array ary);
  CMyComPtr<IInStream> get_stream(const wchar_t* name);
  CMyComPtr<IInStream> get_stream_by_index(size_t index);
  const std::unique_ptr<UString>& get_name(size_t index);

private:
  IInStream* load_stream(const wchar_t* name);
};

} // namespace n7zip
