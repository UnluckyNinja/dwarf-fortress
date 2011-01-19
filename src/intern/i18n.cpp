/**
 * @file
 * @date 17 juil. 2010
 * @todo comment
 */

#include "intern/i18n.hpp"

namespace i18n {
  i18nostream& operator<<(i18nostream& out, const ::std::string& s) {
    if (!out.know(s)) {
      out.add(s);
      //      __infoM(i18n)
      //        << s;
    }

    return out;
  }

  i18nostream out;
}
