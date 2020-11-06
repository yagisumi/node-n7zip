#include "debug.h"

#ifdef DEBUG
  #ifdef _WIN32
void
TRACE(const char* fmt, ...)
{
  std::va_list list;
  std::va_list list2;
  va_start(list, fmt);
  va_copy(list2, list);

  int len = std::vsnprintf(NULL, 0, fmt, list);
  if (len > 0) {
    // char* buf = (char*)malloc((len + 1) * sizeof(char));
    auto buf = std::make_unique<char[]>(len + 1);
    std::vsnprintf(buf.get(), len + 1, fmt, list2);
    OutputDebugStringA(buf.get());
    // free(buf);
  }

  va_end(list2);
  va_end(list);
}

  #else
void
TRACE(const char* fmt, ...)
{
  std::va_list list;
  va_start(list, fmt);
  std::vfprintf(stderr, fmt, list);
  std::fputs("\n", stderr);
  va_end(list);
}

  #endif
#endif
