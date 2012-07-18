#include "g_src/platform.h"

#include <iostream>

#ifndef WIN32
int MessageBox(HWND* dummy, const char* text, const char* caption, UINT type) {
  std::clog << text << "\n";
  return IDNO;
}
#endif
