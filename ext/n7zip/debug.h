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

  #ifdef _WIN32
    #define TRACE_P(fmt, ...) TRACE("0x%p: " fmt, this, ##__VA_ARGS__)
  #else
    #define TRACE_P(fmt, ...) TRACE("%018p: " fmt, this, ##__VA_ARGS__)
  #endif

class n7zipMarker
{
public:
  n7zipMarker() { TRACE("n7zip start"); }
  ~n7zipMarker() { TRACE("n7zip end"); }
};
#else
  #define TRACE(...)
  #define TRACE_P(...)
#endif
