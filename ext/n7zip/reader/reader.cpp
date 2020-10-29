#include "reader.h"

namespace n7zip {

Reader::Reader(int fmt_index,
               CMyComPtr<IInArchive>& archive,
               CMyComPtr<IArchiveOpenCallback>& open_callback)
  : m_fmt_index(fmt_index)
  , m_archive(archive)
  , m_open_callback(open_callback)
{
  TRACE("+ Reader %p", this);
  m_closed.store(false);
}

Reader::~Reader()
{
  TRACE("- Reader %p", this);
  if (!m_closed.load()) {
    close();
  }
}

std::unique_lock<std::recursive_mutex>
Reader::lock()
{
  return std::unique_lock<std::recursive_mutex>(m_mutex);
}

bool
Reader::close()
{
  if (!m_closed) {
    auto locked = lock();
    auto r = m_archive->Close();
    if (r == S_OK) {
      m_closed.store(true);
    } else {
      TRACE("[Reader::close] false");
      return false;
    }
  }

  TRACE("[Reader::close] true");
  return true;
}

} // namespace n7zip
