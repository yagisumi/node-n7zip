#pragma once

#include "common.h"

namespace n7zip {

#ifdef _WIN32
typedef HMODULE TLIB;

template<typename T>
inline T
MyGetProcAddress(TLIB lib, const char* symbol)
{
  return reinterpret_cast<T>(GetProcAddress(lib, symbol));
}
#else
  #include <dlfcn.h>
typedef void* TLIB;

template<typename T>
inline T
MyGetProcAddress(TLIB lib, const char* symbol)
{
  return reinterpret_cast<T>(dlsym(lib, symbol));
}
#endif

class Library
{
  TLIB m_lib;
  UInt32 m_num_formats = 0;
  UInt32 m_num_methods = 0;

public:
  Func_CreateObject CreateObject;
  Func_GetNumberOfFormats GetNumberOfFormats;
  Func_GetHandlerProperty2 GetHandlerProperty2;
  Func_GetNumberOfMethods GetNumberOfMethods;
  Func_GetMethodProperty GetMethodProperty;
  Func_CreateDecoder CreateDecoder;
  Func_CreateEncoder CreateEncoder;
  Func_SetCodecs SetCodecs;

  Library(TLIB lib);
  ~Library();
  inline bool HasFormats()
  {
    return this->CreateObject && GetNumberOfFormats && GetHandlerProperty2 && m_num_formats > 0;
  }
  inline bool HasMethods() { return GetNumberOfMethods && GetMethodProperty && CreateDecoder && m_num_methods > 0; }
  inline UInt32 NumberOfFormats() { return m_num_formats; }
  inline UInt32 NumberOfMethods() { return m_num_methods; }
};

class Format
{
public:
  UInt32 Index;
  UInt32 LibIndex;
  UInt32 LibFmtIndex;
  UInt32 Flags = 0;
  GUID ClassId;
  std::string Name;
  bool CanUpdate = false;
  bool KeepName() const { return (Flags & NArcInfoFlags::kKeepName) != 0; }
  bool FindSignature() const { return (Flags & NArcInfoFlags::kFindSignature) != 0; }
  bool AltStreams() const { return (Flags & NArcInfoFlags::kAltStreams) != 0; }
  bool NtSecure() const { return (Flags & NArcInfoFlags::kNtSecure) != 0; }
  bool SymLinks() const { return (Flags & NArcInfoFlags::kSymLinks) != 0; }
  bool HardLinks() const { return (Flags & NArcInfoFlags::kHardLinks) != 0; }
  bool UseGlobalOffset() const { return (Flags & NArcInfoFlags::kUseGlobalOffset) != 0; }
  bool StartOpen() const { return (Flags & NArcInfoFlags::kStartOpen) != 0; }
  bool BackwardOpen() const { return (Flags & NArcInfoFlags::kBackwardOpen) != 0; }
  bool PreArc() const { return (Flags & NArcInfoFlags::kPreArc) != 0; }
  bool PureStartOpen() const { return (Flags & NArcInfoFlags::kPureStartOpen) != 0; }
};

class Method
{
public:
  UInt32 Index;
  UInt32 LibIndex;
  UInt32 LibMtdIndex;
  std::string Name;
  CMethodId Id;
  bool DecoderIsAssigned;
  bool EncoderIsAssigned;
};

class LibraryInfo
  : public ICompressCodecsInfo
  , public CMyUnknownImp
{
  std::vector<std::unique_ptr<Library>> m_libraries;
  std::vector<std::unique_ptr<Format>> m_formats;
  std::vector<std::unique_ptr<Method>> m_methods;
  std::set<GUID> m_format_ids;
  std::set<CMethodId> m_method_ids;

public:
  LibraryInfo() { TRACE("+ LibraryInfo %p", this); }
  virtual ~LibraryInfo() { TRACE("- LibraryInfo %p", this); }

  MY_UNKNOWN_IMP1(ICompressCodecsInfo)

  STDMETHOD(GetNumMethods)(UInt32* numMethods);
  STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT* value);
  STDMETHOD(CreateDecoder)(UInt32 index, const GUID* iid, void** coder);
  STDMETHOD(CreateEncoder)(UInt32 index, const GUID* iid, void** coder);
  HRESULT AddLibrary(Napi::String& path);
  Napi::Array GetFormats(const Napi::CallbackInfo& info);
  Napi::Array GetCodecs(const Napi::CallbackInfo& info);

private:
  void LoadFormats(std::unique_ptr<Library>& library);
  void LoadMethods(std::unique_ptr<Library>& library);
};

extern CMyComPtr<LibraryInfo> g_library_info;

Napi::Object
loadLibrary(const Napi::CallbackInfo& info);

Napi::Array
getFormats(const Napi::CallbackInfo& info);

Napi::Array
getCodecs(const Napi::CallbackInfo& info);

} // namespace n7zip
