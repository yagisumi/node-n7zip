#pragma once

#include "n7zip/common.h"

namespace n7zip {

struct InStreamData
{
  InStreamData(std::unique_ptr<UString>&& _name, CMyComPtr<IInStream>& _stream)
    : name(std::move(_name))
    , stream(_stream)
  {}
  std::unique_ptr<UString> name;
  CMyComPtr<IInStream> stream;
};

class OpenCallback
  : public IArchiveOpenCallback
  , public IArchiveOpenVolumeCallback
  , public ICryptoGetTextPassword
  , public CMyUnknownImp
{
public:
  std::unique_ptr<std::vector<InStreamData>> m_streams;

  OpenCallback(std::unique_ptr<std::vector<InStreamData>>&& streams);
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
