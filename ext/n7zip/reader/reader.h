#pragma once

#include "../common.h"
#include "../callbacks/open_callback.h"

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

class Reader
{
  int m_fmt_index;
  CMyComPtr<IInArchive> m_archive;
  CMyComPtr<IArchiveOpenCallback> m_open_callback;
  std::recursive_mutex m_mutex;

public:
  UInt32 m_num_of_items;
  UInt32 m_num_of_arc_props;
  UInt32 m_num_of_props;
  std::atomic<bool> m_closed;
  Napi::ObjectReference m_wrap;

  Reader(int fmt_index,
         CMyComPtr<IInArchive>& archive,
         CMyComPtr<IArchiveOpenCallback>& open_callback);
  ~Reader();

  std::unique_lock<std::recursive_mutex> lock();
  HRESULT close();
  std::unique_ptr<ReaderPropertyInfo> get_property_info();
};

} // namespace n7zip
