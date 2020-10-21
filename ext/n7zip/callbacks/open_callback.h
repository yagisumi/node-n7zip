#pragma once

#include "n7zip/common.h"

namespace n7zip {

class OpenCallback
  : public IArchiveOpenCallback
  , public IArchiveOpenVolumeCallback
  , public ICryptoGetTextPassword
  , public CMyUnknownImp
{
  OpenCallback();
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
