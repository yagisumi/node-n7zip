#include "open_callback.h"

namespace n7zip {

OpenCallback::OpenCallback()
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
  return E_NOTIMPL;
}

STDMETHODIMP
OpenCallback::GetStream(const wchar_t* name, IInStream** inStream)
{
  TRACE("[OpenCallback::GetStream]");
  return E_NOTIMPL;
}

STDMETHODIMP
OpenCallback::CryptoGetTextPassword(BSTR* password)
{
  TRACE("[OpenCallback::CryptoGetTextPassword]");
  return E_NOTIMPL;
}

} // namespace n7zip
