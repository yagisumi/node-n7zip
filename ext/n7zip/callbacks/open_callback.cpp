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
  TRACE("[OpenCallback::SetTotal]");
  return S_OK;
}

STDMETHODIMP
OpenCallback::SetCompleted(const UInt64* files, const UInt64* bytes)
{
  TRACE("[OpenCallback::SetCompleted]");
  return S_OK;
}

STDMETHODIMP
OpenCallback::GetProperty(PROPID propID, PROPVARIANT* value)
{
  TRACE("[OpenCallback::GetProperty]");
  if (propID != kpidName) {
    return E_NOTIMPL;
  }

  NWindows::NCOM::PropVariant_Clear(value);
  value->vt = VT_BSTR;
  auto& name = m_streams->at(0).name;
  value->bstrVal = ::SysAllocStringLen(name->Ptr(), name->Len());

  if (value->bstrVal) {
    return S_OK;
  } else {
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
      m_streams->at(i).stream->AddRef();
      *inStream = m_streams->at(i).stream;
      return S_OK;
    }
  }
  return E_FAIL;
}

STDMETHODIMP
OpenCallback::CryptoGetTextPassword(BSTR* password)
{
  TRACE("[OpenCallback::CryptoGetTextPassword]");
  return E_NOTIMPL;
}

} // namespace n7zip
