#pragma once

#include "../common.h"
#include "../callbacks/open_callback.h"

namespace n7zip {

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
};

} // namespace n7zip
