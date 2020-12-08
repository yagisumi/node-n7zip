#pragma once

#include "../base.h"

#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)

inline bool
operator<(REFGUID g1, REFGUID g2)
{
  for (size_t i = 0; i < (sizeof(g1) / sizeof(int)); i++)
    if (((unsigned int*)&g1)[i] != ((unsigned int*)&g2)[i])
      return ((unsigned int*)&g1)[i] < ((unsigned int*)&g2)[i];
  return false;
}

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
