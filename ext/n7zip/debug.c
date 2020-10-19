#include "debug.h"

#ifdef DEBUG
  #ifdef _WIN32
void
TRACE(const char* fmt, ...)
{
  char buf[256] = { 0 };
  va_list list;
  va_start(list, fmt);
  _vsnprintf(buf, 127, fmt, list);
  OutputDebugStringA(buf);
  va_end(list);
}

  #else
void
TRACE(const char* fmt, ...)
{
  va_list list;
  va_start(list, fmt);
  vfprintf(stderr, fmt, list);
  fputs("\n", stderr);
  va_end(list);
}

  #endif
#endif
