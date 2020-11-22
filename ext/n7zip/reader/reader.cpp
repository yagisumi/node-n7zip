#include "reader.h"

namespace n7zip {

Reader::Reader(int fmt_index,
               CMyComPtr<IInArchive>& archive,
               CMyComPtr<IArchiveOpenCallback>& open_callback)
  : m_fmt_index(fmt_index)
  , m_archive(archive)
  , m_open_callback(open_callback)
{
  TRACE_P("+ Reader");
  m_archive->GetNumberOfItems(&m_num_of_items);
  m_archive->GetNumberOfArchiveProperties(&m_num_of_arc_props);
  m_archive->GetNumberOfProperties(&m_num_of_props);
  m_closed.store(false);
}

Reader::~Reader()
{
  TRACE_P("- Reader");
  if (!m_closed.load()) {
    close();
  }
}

std::unique_lock<std::recursive_mutex>
Reader::lock()
{
  return std::unique_lock<std::recursive_mutex>(m_mutex);
}

HRESULT
Reader::close()
{
  if (!m_closed) {
    auto locked = lock();
    auto r = m_archive->Close();
    if (r == S_OK) {
      m_closed.store(true);
    }
    return r;
  } else {
    return S_OK;
  }
}

std::unique_ptr<ReaderPropertyInfo>
Reader::get_property_info()
{
  auto locked = lock();
  if (m_closed) {
    return std::unique_ptr<ReaderPropertyInfo>();
  }

  auto info = std::make_unique<ReaderPropertyInfo>();

  info->archive.resize(m_num_of_arc_props);
  info->entry.resize(m_num_of_props);

  for (UInt32 i = 0; i < m_num_of_arc_props; i++) {
    m_archive->GetArchivePropertyInfo(
      i, &info->archive[i].name, &info->archive[i].pid, &info->archive[i].type);
  }

  for (UInt32 i = 0; i < m_num_of_props; i++) {
    m_archive->GetPropertyInfo(i, &info->entry[i].name, &info->entry[i].pid, &info->entry[i].type);
  }

  return info;
}

std::vector<EntryProperty>
Reader::get_archive_properties()
{
  std::vector<EntryProperty> props;

  auto locked = lock();
  if (m_closed) {
    return props;
  }

  props.resize(m_num_of_arc_props);

  for (UInt32 i = 0; i < m_num_of_arc_props; i++) {
    VARTYPE ver_type;
    CMyComBSTR2 name;
    m_archive->GetArchivePropertyInfo(i, &name, &props[i].prop_id, &ver_type);
    m_archive->GetArchiveProperty(props[i].prop_id, &props[i].prop);
  }

  return props;
}

std::vector<Entry>
Reader::get_entries(UInt32 start, UInt32 end, std::vector<PROPID>& prop_ids)
{
  std::vector<Entry> entries;

  auto locked = lock();
  if (m_closed) {
    return entries;
  }

  for (UInt32 i = start; i < end; i++) {
    std::vector<EntryProperty> props(prop_ids.size());
    for (size_t pidx = 0; pidx < prop_ids.size(); pidx++) {
      props[pidx].prop_id = prop_ids[pidx];
      m_archive->GetProperty(i, prop_ids[pidx], &props[pidx].prop);
    }

    entries.emplace_back(i, std::move(props));
  }

  return entries;
}

} // namespace n7zip
