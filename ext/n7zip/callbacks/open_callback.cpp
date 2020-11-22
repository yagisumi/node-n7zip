#include "open_callback.h"

namespace n7zip {

OpenCallback::OpenCallback(std::unique_ptr<InStreams>&& streams,
                           std::unique_ptr<UString>&& password)
  : m_streams(std::move(streams))
  , m_password(std::move(password))
{
  TRACE_THIS("+ OpenCallback ");
}

OpenCallback::~OpenCallback()
{
  TRACE_THIS("- OpenCallback ");
}

STDMETHODIMP
OpenCallback::SetTotal(const UInt64* files, const UInt64* bytes)
{
#ifdef DEBUG
  if (files && bytes) {
    TRACE_THIS("[OpenCallback::SetTotal] files: %llu, bytes: %llu", *files, *bytes);
  } else if (files) {
    TRACE_THIS("[OpenCallback::SetTotal] files: %llu, bytes: NULL", *files);
  } else if (bytes) {
    TRACE_THIS("[OpenCallback::SetTotal] files: NULL, bytes: %llu", *bytes);
  } else {
    TRACE_THIS("[OpenCallback::SetTotal] files: NULL, bytes: NULL");
  }
#endif
  return S_OK;
}

STDMETHODIMP
OpenCallback::SetCompleted(const UInt64* files, const UInt64* bytes)
{
#ifdef DEBUG
  if (files && bytes) {
    TRACE_THIS("[OpenCallback::SetTotal] files: %llu, bytes: %llu", *files, *bytes);
  } else if (files) {
    TRACE_THIS("[OpenCallback::SetTotal] files: %llu, bytes: NULL", *files);
  } else if (bytes) {
    TRACE_THIS("[OpenCallback::SetTotal] files: NULL, bytes: %llu", *bytes);
  } else {
    TRACE_THIS("[OpenCallback::SetTotal] files: NULL, bytes: NULL");
  }
#endif
  return S_OK;
}

STDMETHODIMP
OpenCallback::GetProperty(PROPID propID, PROPVARIANT* value)
{
  TRACE_THIS("[OpenCallback::GetProperty]");
  if (propID != kpidName) {
    TRACE_THIS("propID != kpidName");
    return E_NOTIMPL;
  }

  NWindows::NCOM::PropVariant_Clear(value);
  auto& name = m_streams->get_name(0);
  if (name) {
    value->vt = VT_BSTR;
    value->bstrVal = ::SysAllocStringLen(name->Ptr(), name->Len());
  }

  if (value->bstrVal) {
    TRACE_THIS("S_OK");
    return S_OK;
  } else {
    TRACE_THIS("E_OUTOFMEMORY");
    return E_OUTOFMEMORY;
  }
}

STDMETHODIMP
OpenCallback::GetStream(const wchar_t* name, IInStream** inStream)
{
  TRACE_THIS("[OpenCallback::GetStream]");

  auto stream = m_streams->get_stream_by_name(name);
  if (stream) {
    *inStream = stream.Detach();
    return S_OK;
  }

  return S_FALSE;
}

STDMETHODIMP
OpenCallback::CryptoGetTextPassword(BSTR* password)
{
  TRACE_THIS("[OpenCallback::CryptoGetTextPassword]");

  if (m_password) {
    *password = SysAllocStringLen(m_password->Ptr(), m_password->Len());
    TRACE_THIS("S_OK");
    return S_OK;
  }

  TRACE_THIS("E_ABORT");
  return E_ABORT;
}

} // namespace n7zip
