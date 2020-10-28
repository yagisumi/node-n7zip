#pragma once

#include "../common.h"
#include "../streams/in_streams.h"

namespace n7zip {

class OpenCallback
  : public IArchiveOpenCallback
  , public IArchiveOpenVolumeCallback
  , public ICryptoGetTextPassword
  , public CMyUnknownImp
{
public:
  std::unique_ptr<InStreams> m_streams;
  std::unique_ptr<UString> m_password;

  OpenCallback(std::unique_ptr<InStreams>&& streams, std::unique_ptr<UString>&& password);
  ~OpenCallback();

  MY_UNKNOWN_IMP3( //
    IArchiveOpenCallback,
    IArchiveOpenVolumeCallback,
    ICryptoGetTextPassword)

  // INTERFACE_IArchiveOpenCallback(;)
  STDMETHOD(SetTotal)(const UInt64* files, const UInt64* bytes);
  STDMETHOD(SetCompleted)(const UInt64* files, const UInt64* bytes);

  // INTERFACE_IArchiveOpenVolumeCallback(;)
  STDMETHOD(GetProperty)(PROPID propID, PROPVARIANT* value);
  STDMETHOD(GetStream)(const wchar_t* name, IInStream** inStream);

  // ICryptoGetTextPassword
  STDMETHOD(CryptoGetTextPassword)(BSTR* password);
};

} // namespace n7zip
