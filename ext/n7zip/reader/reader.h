#pragma once

#include "../common.h"

namespace n7zip {

struct PropertyInfo
{
  CMyComBSTR2 name;
  PROPID pid;
  VARTYPE type;
  PropertyInfo() {}
  PropertyInfo(const PropertyInfo& other)
  {
    name = other.name;
    pid = other.pid;
    type = other.type;
  };
};

struct ReaderPropertyInfo
{
  std::vector<PropertyInfo> archive;
  std::vector<PropertyInfo> entry;
};

struct GetEntriesOption
{
  uint32_t limit;
  uint32_t start;
  uint32_t end;
  std::vector<PROPID> prop_ids;
};

struct EntryProperty
{
  PROPID prop_id;
  NWindows::NCOM::CPropVariant prop;
};

struct Entry
{
  UInt32 index;
  std::vector<EntryProperty> props;
  Entry(UInt32 index, std::vector<EntryProperty>&& props)
    : index(index)
    , props(std::move(props))
  {}
};

class Reader : public Napi::ObjectWrap<Reader>
{
  int m_fmt_index;
  std::string m_fmt_name;
  CMyComPtr<IInArchive> m_archive;
  CMyComPtr<IArchiveOpenCallback> m_open_callback;
  mutable std::recursive_mutex m_mutex;

public:
  UInt32 m_num_of_items;
  UInt32 m_num_of_arc_props;
  UInt32 m_num_of_props;
  std::atomic<bool> m_closed;
  Napi::ObjectReference m_wrap;

  Reader(const Napi::CallbackInfo& info);
  ~Reader();

  static Napi::FunctionReference constructor;
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  static Napi::Object New(Napi::Env,
                          int fmt_index,
                          std::string&& fmt_name,
                          CMyComPtr<IInArchive>& archive,
                          CMyComPtr<IArchiveOpenCallback>& open_callback);

  Napi::Value GetFormatIndex(const Napi::CallbackInfo& info);
  Napi::Value GetFormatName(const Napi::CallbackInfo& info);
  Napi::Value IsClosed(const Napi::CallbackInfo& info);
  Napi::Value GetNumberOfEntries(const Napi::CallbackInfo& info);
  Napi::Value GetNumberOfArchiveProperties(const Napi::CallbackInfo& info);
  Napi::Value GetNumberOfProperties(const Napi::CallbackInfo& info);
  Napi::Value Close(const Napi::CallbackInfo& info);
  Napi::Value GetPropertyInfo(const Napi::CallbackInfo& info);
  Napi::Value GetArchiveProperties(const Napi::CallbackInfo& info);
  Napi::Value GetEntries(const Napi::CallbackInfo& info);
  Napi::Value Extract(const Napi::CallbackInfo& info);

  std::unique_lock<std::recursive_mutex> acquire_lock() const;
  HRESULT close();
  bool is_closed() const;
  std::unique_ptr<ReaderPropertyInfo> get_property_info();
  std::vector<EntryProperty> get_archive_properties(std::unique_ptr<std::vector<PROPID>>& prop_ids);
  std::vector<Entry> get_entries(UInt32 start,
                                 UInt32 end,
                                 std::unique_ptr<std::vector<PROPID>>& prop_ids);
  HRESULT extract(const UInt32* indices,
                  UInt32 numItems,
                  Int32 testMode,
                  IArchiveExtractCallback* extractCallback);
  bool is_dir(const UInt32 index);
  bool Reader::is_link(const UInt32 index);
};

Napi::Object
InitReader(Napi::Env env, Napi::Object exports);

} // namespace n7zip
