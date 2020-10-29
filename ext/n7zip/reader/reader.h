#pragma once

#include "../common.h"
#include "../callbacks/open_callback.h"

namespace n7zip {

class Reader
{
public:
  Reader(int fmt_index,
         CMyComPtr<IInArchive>& archive,
         CMyComPtr<IArchiveOpenCallback>& open_callback);
  ~Reader();
  std::atomic<bool> m_closed;
  Napi::ObjectReference m_wrap;
  bool close();

private:
  int m_fmt_index;
  CMyComPtr<IInArchive> m_archive;
  CMyComPtr<IArchiveOpenCallback> m_open_callback;
  std::recursive_mutex m_mutex;
  inline std::unique_lock<std::recursive_mutex> lock()
  {
    return std::unique_lock<std::recursive_mutex>(m_mutex);
  }
};

class ReaderWrap : public Napi::ObjectWrap<ReaderWrap>
{
public:
  static Napi::FunctionReference constructor;
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  std::unique_ptr<Reader> m_reader;
  ReaderWrap(const Napi::CallbackInfo& info);
  ~ReaderWrap();
  Napi::Value isClosed(const Napi::CallbackInfo& info);
  Napi::Value close(const Napi::CallbackInfo& info);
};

Napi::Object
InitReader(Napi::Env env, Napi::Object exports);

} // namespace n7zip
