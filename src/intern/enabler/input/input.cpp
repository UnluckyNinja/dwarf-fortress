/**
 * @file
 * @date 14 juin 2010
 * @todo comment
 */

#include "intern/enabler/input/input.hpp"

#include "intern/enabler/input/event.hpp"
#include "intern/enabler/input/event_match.hpp"

// Used to decide which key-binding to display. As a heuristic, we
// prefer whichever display string is shortest.
bool less_sz::operator()(const ::std::string &a, const ::std::string &b) const {
  if (a.size() < b.size()) {
    return true;
  }

  if (a.size() > b.size()) {
    return false;
  }

  return a < b;
}

// Decodes an UTF-8 encoded string into a /single/ UTF-8 character,
// discarding any overflow. Returns 0 on parse error.
int32_t decode_utf8(const ::std::string &s) {
  int unicode = 0;
  int length;
  int i;

  if (s.length() == 0) {
    return 0;
  }

  length = decode_utf8_predict_length(s[0]);
  switch (length) {
    case 1:
      unicode = s[0];
      break;
    case 2:
      unicode = s[0] & 0x1f;
      break;
    case 3:
      unicode = s[0] & 0x0f;
      break;
    case 4:
      unicode = s[0] & 0x07;
      break;
    default:
      return 0;
  }

  // Concatenate the follow-up bytes
  if (static_cast< int32_t > (s.length()) < length)
    return 0;
  for (i = 1; i < length; i++) {
    if ((s[i] & 0xc0) != 0x80)
      return 0;
    unicode = (unicode << 6) | (s[i] & 0x3f);
  }
  return unicode;
}

// Returns the length of an utf-8 sequence, based on its first byte
int32_t decode_utf8_predict_length(int8_t byte) {
  if ((byte & 0x80) == 0)
    return 1;
  if ((byte & 0xe0) == 0xc0)
    return 2;
  if ((byte & 0xf0) == 0xe0)
    return 3;
  if ((byte & 0xf8) == 0xf0)
    return 4;
  return 0; // Invalid start byte
}

// Encode an arbitrary unicode value as a string. Returns an empty
// string if the value is out of range.
::std::string encode_utf8(int32_t unicode) {
  ::std::string s;
  int i;
  if (unicode < 0 || unicode > 0x10ffff)
    return ""; // Out of range for utf-8
  else if (unicode <= 0x007f) { // 1-byte utf-8
    s.resize(1, 0);
  } else if (unicode <= 0x07ff) { // 2-byte utf-8
    s.resize(2, 0);
    s[0] = 0xc0;
  } else if (unicode <= 0xffff) { // 3-byte utf-8
    s.resize(3, 0);
    s[0] = 0xe0;
  } else { // 4-byte utf-8
    s.resize(4, 0);
    s[0] = 0xf0;
  }

  // Build up the string, right to left
  for (i = s.length() - 1; i > 0; i--) {
    s[i] = 0x80 | (unicode & 0x3f);
    unicode >>= 6;
  }
  // Finally, what's left goes in the low bits of s[0]
  s[0] |= unicode;
  return s;
}
