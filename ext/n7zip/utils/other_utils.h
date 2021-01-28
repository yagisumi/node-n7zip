#pragma once

#include "../base.h"

inline bool
operator<(REFGUID g1, REFGUID g2)
{
  for (size_t i = 0; i < (sizeof(g1) / sizeof(int)); i++)
    if (((unsigned int*)&g1)[i] != ((unsigned int*)&g2)[i])
      return ((unsigned int*)&g1)[i] < ((unsigned int*)&g2)[i];
  return false;
}

// LARGE_INTEGER
#ifdef WIN32
typedef LARGE_INTEGER N7ZIP_LARGE_INTEGER;
#else
typedef union _N7ZIP_LARGE_INTEGER
{
  struct
  {
    DWORD LowPart;
    LONG HighPart;
  };
  struct
  {
    DWORD LowPart;
    LONG HighPart;
  } u;
  LONGLONG QuadPart;
} N7ZIP_LARGE_INTEGER;
#endif

// HRESULT

#ifndef SUCCEEDED
  #define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)
#endif

#ifndef MAKE_HRESULT
  #define MAKE_HRESULT(sev, fac, code)                                       \
    ((HRESULT)(((unsigned long)(sev) << 31) | ((unsigned long)(fac) << 16) | \
               ((unsigned long)(code))))
#endif

#ifndef SEVERITY_ERROR
  #define SEVERITY_ERROR 1
#endif

#ifndef FACILITY_ITF
  #define FACILITY_ITF 0x04
#endif

namespace n7zip {

enum N7zipResult : HRESULT
{
  E_CLOSED = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 1)
};

} // namespace n7zip
