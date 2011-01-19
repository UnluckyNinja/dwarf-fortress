/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/texture/full_id.hpp"

bool texture_fullid::operator<(const texture_fullid& other) const {
  if (texpos != other.texpos)
    return texpos < other.texpos;
  if (r != other.r)
    return r < other.r;
  if (g != other.g)
    return g < other.g;
  if (b != other.b)
    return b < other.b;
  if (br != other.br)
    return br < other.br;
  if (bg != other.bg)
    return bg < other.bg;
  return bb < other.bb;
}
