#pragma once

#ifdef DEBUG
  #ifdef __cplusplus
extern "C"
{
  #endif

  #ifdef _WIN32
    #include <windows.h>
  #else
    #include <unistd.h>
  #endif

  #include <stdarg.h>
  #include <stdio.h>
  void TRACE(const char* fmt, ...);

  #ifdef __cplusplus
}
  #endif
#else
  #define TRACE(...)
#endif
