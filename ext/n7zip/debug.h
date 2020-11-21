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
    #define ADDR "0x%p: "
  #else
    #define ADDR "%018p: "
  #endif
  #define TRACE_P(fmt, ...) TRACE(ADDR fmt, this, ##__VA_ARGS__)
  #define TRACE_ADDR(ptr, fmt, ...) TRACE(ADDR fmt, ptr, ##__VA_ARGS__)

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
