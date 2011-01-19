/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "extern/utils/platform.hpp"

#include <logging/logging.hpp>

int MessageBox(int* dummy, char const* text, char const* caption, unsigned int type) {
  __infoM(messagebox)
    << text;
  return 2;
}

/* Returns milliseconds since 1970
 * Wraps every 24 days (assuming 32-bit signed dwords)
 */
unsigned long GetTickCount() {
  struct timeval tp;
  gettimeofday(&tp, 0);
  return (tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

int CreateDirectory(char const* pathname, void*) {
  try {
    ::boost::filesystem::create_directory(pathname);
  } catch (::boost::filesystem::filesystem_error x) {
    __error
      << x.what();
    return 0;
  }

  return 1;
}

char* itoa(int value, char* result, int base) {
  __info
    << "itoa called with value: " << value << ", base: " << base;

  // check that the base is valid
  if (base < 2 || base > 16) {
    *result = 0;
    return result;
  }

  char* out = result;
  int quot = value;

  do {
    *out = "0123456789abcdef"[ /*std::*/abs(quot % base)];
    ++out;
    quot /= base;
  } while (quot);

  if (value < 0)
    *out++ = '-';

  std::reverse(result, out);
  *out = 0;
  return result;
}
