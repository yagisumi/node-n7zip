#include "reader.h"

namespace n7zip {

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

static Napi::Object
createReader(const Napi::CallbackInfo& info)
{
  auto lock = g_library_info->get_shared_lock();
  auto env = info.Env();

  if ((info.Length() < 2 || !(info[0].IsArray())) || !(info[1].IsArray())) {
    return ERR(env, "invalid arguments", ErrorType::TypeError);
  }

  auto fmt_index_ary = info[1].As<Napi::Array>();
  auto fmt_indices = getFormatIndices(fmt_index_ary);

  if (fmt_indices.size() == 0) {
    return ERR(env, "invalid format index array", ErrorType::TypeError);
  }

  auto stream_ary = info[0].As<Napi::Array>();
  auto streams = std::make_unique<std::vector<InStreamData>>();
  // std::unique_ptr<std::vector<InStreamData>> streams(new std::vector<InStreamData>());

  for (uint32_t i = 0; i < stream_ary.Length(); i++) {
    auto stream_data = stream_ary.Get(i);
    if (stream_data.IsArray()) {
      auto stream_data_ary = stream_data.As<Napi::Array>();
      if (stream_data_ary.Length() < 2) {
        return ERR(env, "invalid stream data", ErrorType::TypeError);
      }

      auto v_name = stream_data_ary.Get((uint32_t)0);
      if (!(v_name.IsString())) {
        return ERR(env, "invalid stream data", ErrorType::TypeError);
      }
      auto name = ConvertNapiStringToUString(v_name.ToString());

      auto data = stream_data_ary.Get((uint32_t)1);
      if (data.IsNumber()) {
        auto num = data.ToNumber();
#ifdef _WIN32
        auto fd = num.Int32Value();
#else
        auto fd = num.Int64Value();
#endif
        if (fd < 0) {
          return ERR(env, "invalid file descriptor", ErrorType::TypeError);
        }

        auto autoclose = true;
        if (stream_data_ary.Length() > 2) {
          auto v_autoclose = stream_data_ary.Get((uint32_t)2);
          if (v_autoclose.IsBoolean()) {
            autoclose = v_autoclose.ToBoolean().Value();
          }
        }

        CMyComPtr<IInStream> stream(new FdInStream(fd, autoclose));
        streams->emplace_back(std::move(name), stream);
      } else if (data.IsString()) {
        auto str = data.ToString();
        auto path = str.Utf8Value();
        uv_fs_t open_req;
        auto fd = uv_fs_open(nullptr, &open_req, path.c_str(), UV_FS_O_RDONLY, 0666, nullptr);
        if (fd < 0) {
          return ERR(env, "failed to open file");
        }

        CMyComPtr<IInStream> stream(new FdInStream(fd, true));
        streams->emplace_back(std::move(name), stream);
      } else {
        return ERR(env, "unexpected strea", ErrorType::TypeError);
      }
    }
  }

  if (streams->empty()) {
    return ERR(env, "empty stream data", ErrorType::TypeError);
  }

  CMyComPtr<OpenCallback> open_callback(new OpenCallback(std::move(streams)));
  GUID interface_guid = IID_IInArchive;

  for (auto i : fmt_indices) {
    CMyComPtr<IInArchive> archive;
    auto r = g_library_info->create_object(i, &interface_guid, (void**)&archive);
    TRACE("fmt: %d, r: %d", i, r);
    if (r == S_OK) {
      auto r_open = archive->Open(open_callback->m_streams->at(0).stream, 0, open_callback);
      TRACE("r_open: %d", r_open);
      if (r_open == S_OK) {
        archive->Close();
      }
    }
  }

  return OK(env);
}

Napi::Object
InitReader(Napi::Env env, Napi::Object exports)
{
  exports.Set("createReader", Napi::Function::New(env, createReader));

  return exports;
}

} // namespace n7zip
