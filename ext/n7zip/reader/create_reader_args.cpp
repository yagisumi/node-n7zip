#include "create_reader_args.h"
#include "../library.h"
#include "../streams/fd_in_stream.h"
#include "../streams/buffer_in_stream.h"
#include "../streams/multi_in_stream.h"

namespace n7zip {

result<IInStream>
InStreamArg::createInStream()
{
  if (type == InStreamType::Fd) {
    type = InStreamType::None;
    return FdInStream::New(fd, AutoClose);
  } else if (type == InStreamType::Path) {
    type = InStreamType::None;
    return FdInStream::New(path.c_str());
  } else if (type == InStreamType::Buffer) {
    type = InStreamType::None;
    return BufferInStream::New(std::move(buf_ref));
  } else if (type == InStreamType::BufferCopy) {
    type = InStreamType::None;
    return BufferInStream::New(copied_buf.release(), buf_len);
  } else if (type == InStreamType::Multi) {
    type = InStreamType::None;
    return MultiInStream::New(std::move(streams));
  } else {
    return err<IInStream>("Uninitialized or already used to create stream");
  }
}

static result<std::vector<std::unique_ptr<InStreamArg>>>
buildStreamsArg(Napi::Array ary, bool in_streams);

static result<InStreamArg>
buildFdInStreamArg(Napi::Object obj)
{
  auto v_source = obj.Get("source");
  if (!v_source.IsNumber()) {
    return type_err<InStreamArg>("The source of fd type must be Number");
  }

  // int32_t or int64_t
  uv_file fd = v_source.ToNumber();
  // #ifdef _WIN32
  //   uv_file fd = v_source.ToNumber().Int32Value();
  // #else
  //   uv_file fd = v_source.ToNumber().Int64Value();
  // #endif
  bool AutoClose = true;
  auto v_AutoClose = obj.Get("AutoClose");
  if (v_AutoClose.IsBoolean()) {
    AutoClose = v_AutoClose.ToBoolean().Value();
  }

  return ok(new InStreamArg(fd, AutoClose));
}

static result<InStreamArg>
buildPathInSteamArg(Napi::Object obj)
{
  auto v_source = obj.Get("source");
  if (!v_source.IsString()) {
    return type_err<InStreamArg>("The source of path type must be String");
  }

  return ok(new InStreamArg(v_source.ToString().Utf8Value()));
}

static result<InStreamArg>
buildBufferStreamArg(Napi::Object obj)
{
  auto v_source = obj.Get("source");
  if (!v_source.IsBuffer()) {
    return type_err<InStreamArg>("The source of buffer type must be Buffer");
  }
  auto buf = v_source.As<Napi::Buffer<char>>();

  auto v_ShareBuffer = obj.Get("ShareBuffer");
  bool ShareBuffer = false;
  if (v_ShareBuffer.IsBoolean()) {
    ShareBuffer = v_ShareBuffer.ToBoolean().Value();
  }

  if (ShareBuffer) {
    return ok(new InStreamArg(Napi::Persistent(buf)));
  } else {
    auto len = buf.Length();
    auto tmp = std::make_unique<char[]>(len);
    TRACE("0x%p: + buffer", tmp.get());
    std::memcpy(tmp.get(), buf.Data(), len);
    return ok(new InStreamArg(std::move(tmp), len));
  }
}

static result<InStreamArg>
buildMultiStreamArg(Napi::Object obj)
{
  auto v_source = obj.Get("source");
  if (!v_source.IsArray()) {
    return type_err<InStreamArg>("The source of multi type must be Array");
  }

  auto streams = v_source.As<Napi::Array>();
  auto r = buildStreamsArg(streams, false);
  if (r.err()) {
    return err<InStreamArg>(r.move_err());
  } else if (!r.ok()) {
    return err<InStreamArg>("Unexpected error");
  }

  return ok(new InStreamArg(r.move_ok()));
}

result<InStreamArg>
buildInStreamArg(Napi::Object obj, bool in_streams)
{
  auto v_type = obj.Get("type");
  if (!v_type.IsString()) {
    return type_err<InStreamArg>("No stream type is specified");
  }

  auto type = v_type.ToString().Utf8Value();

  std::unique_ptr<InStreamArg> stream;
  result<InStreamArg> r((InStreamArg*)nullptr);

  if (type == "fd") {
    r = buildFdInStreamArg(obj);
  } else if (type == "path") {
    r = buildPathInSteamArg(obj);
  } else if (type == "buffer") {
    r = buildBufferStreamArg(obj);
  } else if (type == "multi") {
    if (in_streams) {
      r = buildMultiStreamArg(obj);
    } else {
      return type_err<InStreamArg>("Multi type in multi type is not supported");
    }
  } else {
    return type_err<InStreamArg>(format("Unkonwn type: %s", type.c_str()));
  }

  if (r.err()) {
    return err<InStreamArg>(r.move_err());
  } else if (!r.ok()) {
    return err<InStreamArg>("Unexpected error");
  }

  std::unique_ptr<UString> name;
  auto v_name = obj.Get("name");
  if (v_name.IsString()) {
    name = ConvertNapiStringToUString(v_name.ToString());
  }
  r.ok()->name = std::move(name);

  return ok(r.move_ok());
}

static result<std::vector<std::unique_ptr<InStreamArg>>>
buildStreamsArg(Napi::Array ary, bool in_streams)
{
  auto prop_name = in_streams ? "streams" : "source";
  auto streams = std::make_unique<std::vector<std::unique_ptr<InStreamArg>>>();

  for (uint32_t i = 0; i < ary.Length(); i++) {
    auto v = ary.Get(i);
    if (v.IsArray() || !v.IsObject()) {
      return type_err<std::vector<std::unique_ptr<InStreamArg>>>(
        format("Invalid stream type (at %s[%u])", prop_name, i));
    }

    auto obj = v.ToObject();
    auto r = buildInStreamArg(obj, in_streams);
    if (r.err()) {
      r.err()->message.append(format(" (at %s[%u])", prop_name, i));
      return err<std::vector<std::unique_ptr<InStreamArg>>>(r.move_err());
    } else if (!r.ok()) {
      return err<std::vector<std::unique_ptr<InStreamArg>>>(
        format("Unexpected error (at %s[%u])", prop_name, i));
    }

    streams->push_back(r.move_ok());
  }

  if (streams->size() == 0) {
    return type_err<std::vector<std::unique_ptr<InStreamArg>>>(format("'%s' is empty", prop_name));
  }

  return ok(std::move(streams));
}

static std::vector<int32_t>
getFormatIndices(Napi::Array fmt_index_ary)
{
  std::vector<int32_t> fmt_indices;
  auto fmt_len = g_library_info->get_formats_length();

  for (uint32_t i = 0; i < fmt_index_ary.Length(); i++) {
    auto elem = fmt_index_ary.Get(i);
    if (elem.IsNumber()) {
      auto num = elem.ToNumber();
      auto n = num.Int32Value();
      if (n >= 0 && n < fmt_len) {
        fmt_indices.push_back(n);
      }
    }
  }

  return fmt_indices;
}

result<CreateReaderArg>
buildCreateReaderArg(Napi::Object arg)
{
  auto cr_arg = std::make_unique<CreateReaderArg>();

  auto v_formats = arg.Get("formats");
  if (!v_formats.IsArray()) {
    return type_err<CreateReaderArg>("'formats' must be Array");
  }

  cr_arg->formats = getFormatIndices(v_formats.As<Napi::Array>());
  if (cr_arg->formats.size() == 0) {
    return type_err<CreateReaderArg>("No valid value for 'formats'");
  }

  auto v_base_dir = arg.Get("baseDir");
  if (v_base_dir.IsString()) {
    cr_arg->base_dir = ConvertNapiStringToUString(v_base_dir.ToString());
  }

  auto v_password = arg.Get("password");
  if (v_password.IsString()) {
    cr_arg->password = ConvertNapiStringToUString(v_password.ToString());
  }

  auto v_streams = arg.Get("streams");
  if (!v_streams.IsArray()) {
    return type_err<CreateReaderArg>("'streams' must be Array");
  }

  auto r = buildStreamsArg(v_streams.As<Napi::Array>(), true);
  if (r.err()) {
    return err<CreateReaderArg>(r.move_err());
  } else if (!r.ok()) {
    return err<CreateReaderArg>("Unexpected error");
  }

  cr_arg->streams = r.move_ok();

  return ok(std::move(cr_arg));
}

} // namespace n7zip
