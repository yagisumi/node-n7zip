#include "open_callback.h"

namespace n7zip {

OpenCallback::OpenCallback(std::unique_ptr<std::vector<InStreamData>>&& streams)
  : m_streams(std::move(streams))
{
  TRACE("+ OpenCallback %p", this);
}

OpenCallback::~OpenCallback()
{
  TRACE("- OpenCallback %p", this);
}

STDMETHODIMP
OpenCallback::SetTotal(const UInt64* files, const UInt64* bytes)
{
#ifdef DEBUG
  if (files && bytes) {
    TRACE("[OpenCallback::SetTotal] files: %llu, bytes: %llu", *files, *bytes);
  } else if (files) {
    TRACE("[OpenCallback::SetTotal] files: %llu, bytes: NULL", *files);
  } else if (bytes) {
    TRACE("[OpenCallback::SetTotal] files: NULL, bytes: %llu", *bytes);
  } else {
    TRACE("[OpenCallback::SetTotal] files: NULL, bytes: NULL");
  }
#endif
  return S_OK;
}

STDMETHODIMP
OpenCallback::SetCompleted(const UInt64* files, const UInt64* bytes)
{
#ifdef DEBUG
  if (files && bytes) {
    TRACE("[OpenCallback::SetTotal] files: %llu, bytes: %llu", *files, *bytes);
  } else if (files) {
    TRACE("[OpenCallback::SetTotal] files: %llu, bytes: NULL", *files);
  } else if (bytes) {
    TRACE("[OpenCallback::SetTotal] files: NULL, bytes: %llu", *bytes);
  } else {
    TRACE("[OpenCallback::SetTotal] files: NULL, bytes: NULL");
  }
#endif
  return S_OK;
}

STDMETHODIMP
OpenCallback::GetProperty(PROPID propID, PROPVARIANT* value)
{
  TRACE("[OpenCallback::GetProperty]");
  if (propID != kpidName) {
    TRACE("propID != kpidName");
    return E_NOTIMPL;
  }

  NWindows::NCOM::PropVariant_Clear(value);
  value->vt = VT_BSTR;
  auto& name = m_streams->at(0).name;
  value->bstrVal = ::SysAllocStringLen(name->Ptr(), name->Len());

  if (value->bstrVal) {
    TRACE("S_OK");
    return S_OK;
  } else {
    TRACE("E_OUTOFMEMORY");
    return E_OUTOFMEMORY;
  }
}

STDMETHODIMP
OpenCallback::GetStream(const wchar_t* name, IInStream** inStream)
{
  TRACE("[OpenCallback::GetStream]");

  for (size_t i = 0; i < m_streams->size(); i++) {
    auto sname = m_streams->at(i).name.get();
    if (*sname == name) {
      TRACE("matched: %u", i);
      // m_streams->at(i).stream->AddRef();
      CMyComPtr<IInStream> tmp = m_streams->at(i).stream;
      // *inStream = m_streams->at(i).stream;
      *inStream = tmp.Detach();
      return S_OK;
    }
  }
  return S_FALSE;
}

STDMETHODIMP
OpenCallback::CryptoGetTextPassword(BSTR* password)
{
  TRACE("[OpenCallback::CryptoGetTextPassword]");
  return E_NOTIMPL;
}

} // namespace n7zip
