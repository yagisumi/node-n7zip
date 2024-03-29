#pragma once

#include "../common.h"
#include <uv.h>

namespace n7zip {

enum class InStreamType
{
  None,
  Fd,
  Path,
  Buffer,
  BufferCopy,
  Multi,
};

struct InStreamArg;

// I apologize for the waste of memory.
struct InStreamArg
{
  // common
  InStreamType type;
  std::unique_ptr<UString> name;
  // Fd
  uv_file fd;
  bool AutoClose;
  InStreamArg(uv_file _fd, bool _AutoClose)
    : type(InStreamType::Fd)
    , fd(_fd)
    , AutoClose(_AutoClose)
  {}
  // Path
  std::string path;
  InStreamArg(std::string&& _path)
    : type(InStreamType::Path)
    , path(std::move(_path))
  {}
  // Buffer
  Napi::Reference<Napi::Buffer<char>> buf_ref;
  InStreamArg(Napi::Reference<Napi::Buffer<char>>&& ref)
    : type(InStreamType::Buffer)
    , buf_ref(std::move(ref))
  {}
  // BufferCopy
  std::unique_ptr<char[]> copied_buf;
  size_t buf_len;
  InStreamArg(std::unique_ptr<char[]>&& buf, size_t len)
    : type(InStreamType::BufferCopy)
    , copied_buf(std::move(buf))
    , buf_len(len)
  {}
  // Multi
  std::unique_ptr<std::vector<std::unique_ptr<InStreamArg>>> streams;
  InStreamArg(std::unique_ptr<std::vector<std::unique_ptr<InStreamArg>>>&& _streams)
    : type(InStreamType::Multi)
    , streams(std::move(_streams))
  {}

  InStreamArg(const InStreamArg& other) = delete;
  InStreamArg& operator=(const InStreamArg& rhs) = delete;
  InStreamArg(InStreamArg&& other) = default;
  InStreamArg& operator=(InStreamArg&& rhs) = default;

  result<IInStream> createInStream();
};

struct CreateReaderArg
{
  std::unique_ptr<UString> base_dir;
  std::unique_ptr<UString> password;
  std::vector<int32_t> formats;
  std::unique_ptr<std::vector<std::unique_ptr<InStreamArg>>> streams;

  CreateReaderArg() = default;
  CreateReaderArg(const CreateReaderArg& other) = delete;
  CreateReaderArg& operator=(const CreateReaderArg& rhs) = delete;
  CreateReaderArg(CreateReaderArg&& other) = default;
  CreateReaderArg& operator=(CreateReaderArg&& rhs) = default;
};

result<InStreamArg>
buildInStreamArg(Napi::Object obj, bool in_streams);

result<CreateReaderArg>
buildCreateReaderArg(Napi::Object arg);

} // namespace n7zip
