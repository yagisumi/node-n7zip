#pragma once

#ifdef DEBUG

  #ifdef _WIN32
    #include <windows.h>
  #else
    #include <unistd.h>
  #endif

  #include <cstdarg>
  #include <cstdio>
  #include <memory>
void
TRACE(const char* fmt, ...);
#else
  #define TRACE(...)
#endif
