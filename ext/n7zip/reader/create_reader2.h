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
using StreamsArg = std::vector<std::unique_ptr<InStreamArg>>;

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
  InStreamArg(std::unique_ptr<char[]>&& buf)
    : type(InStreamType::BufferCopy)
    , copied_buf(std::move(buf))
  {}
  // Multi
  std::unique_ptr<StreamsArg> streams;
  InStreamArg(std::unique_ptr<StreamsArg>&& _streams)
    : type(InStreamType::Multi)
    , streams(std::move(_streams))
  {}

  InStreamArg(const InStreamArg& other) = delete;
  InStreamArg& operator=(const InStreamArg& rhs) = delete;
  InStreamArg(InStreamArg&& other) = default;
  InStreamArg& operator=(InStreamArg&& rhs) = default;
};

struct CreateReaderArg
{
  std::unique_ptr<UString> base_dir;
  std::unique_ptr<UString> password;
  std::vector<int32_t> formats;
  std::unique_ptr<StreamsArg> streams;

  CreateReaderArg() = default;
  CreateReaderArg(const CreateReaderArg& other) = delete;
  CreateReaderArg& operator=(const CreateReaderArg& rhs) = delete;
  CreateReaderArg(CreateReaderArg&& other) = default;
  CreateReaderArg& operator=(CreateReaderArg&& rhs) = default;
};

static result<StreamsArg>
buildStreamsArg(Napi::Array ary, bool in_streams);

Napi::Object
InitReader2(Napi::Env env, Napi::Object exports);

} // namespace n7zip
