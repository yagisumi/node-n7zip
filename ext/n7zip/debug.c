#include "debug.h"

#ifdef DEBUG
  #ifdef _WIN32
void
TRACE(const char* fmt, ...)
{
  va_list list;
  va_list list2;
  va_start(list, fmt);
  va_copy(list2, list);

  int len = vsnprintf(NULL, 0, fmt, list);
  if (len > 0) {
    char* buf = (char*)malloc((len + 1) * sizeof(char));
    vsnprintf(buf, len + 1, fmt, list2);
    OutputDebugStringA(buf);
    free(buf);
  }

  va_end(list2);
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
