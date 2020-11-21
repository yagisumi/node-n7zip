#pragma once

#include "../base.h"
#include <cstdarg>
#include <cstdio>

namespace n7zip {

std::string
format(const char* fmt, ...);

inline std::string
GuidToString(const GUID* guid)
{
  return format( //
    "%08X-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX",
    guid->Data1,
    guid->Data2,
    guid->Data3,
    guid->Data4[0],
    guid->Data4[1],
    guid->Data4[2],
    guid->Data4[3],
    guid->Data4[4],
    guid->Data4[5],
    guid->Data4[6],
    guid->Data4[7]);
}

Napi::String
ConvertBStrToNapiString(Napi::Env env, BSTR bstr);

std::unique_ptr<UString>
ConvertNapiStringToUString(Napi::String str);

class CMyComBSTR2
{
  BSTR m_str;

public:
  CMyComBSTR2()
    : m_str(NULL)
  {}
  ~CMyComBSTR2() { ::SysFreeString(m_str); }
  BSTR* operator&() { return &m_str; }
  operator LPCOLESTR() const { return m_str; }
  operator bool() const { return m_str != NULL; }
  bool operator!() const { return m_str == NULL; }
  operator BSTR() const { return m_str; }

  CMyComBSTR2& operator=(const CMyComBSTR2& src)
  {
    if (m_str != src.m_str) {
      if (m_str)
        ::SysFreeString(m_str);
      m_str = src.MyCopy();
    }
    return *this;
  }

private:
  CMyComBSTR2(LPCOLESTR src) { m_str = ::SysAllocString(src); }
  CMyComBSTR2(const CMyComBSTR2& src) { m_str = src.MyCopy(); }

  CMyComBSTR2& operator=(LPCOLESTR src)
  {
    ::SysFreeString(m_str);
    m_str = ::SysAllocString(src);
    return *this;
  }

  unsigned Len() const { return ::SysStringLen(m_str); }

  BSTR MyCopy() const { return ::SysAllocStringLen(m_str, ::SysStringLen(m_str)); }

  void Empty()
  {
    ::SysFreeString(m_str);
    m_str = NULL;
  }
};

} // namespace n7zip
