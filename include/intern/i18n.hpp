/**
 * @file
 * @date 17 juil. 2010
 * @todo comment
 */

#ifndef INTERN_I18N_HPP_
#define INTERN_I18N_HPP_

#include <iostream>
#include <set>

#include <boost/thread.hpp>

#include <logging/logging.hpp>

namespace i18n {
  struct i18nostream: public ::std::ostream {
    protected:
      ::std::set< ::std::string > known;

    public:
      bool know(const ::std::string& str) {
        return known.count(str) > 0;
      }

      void add(const ::std::string& str) {
        known.insert(str);
      }
  };

  i18nostream& operator<<(i18nostream& out, const ::std::string& s);

  extern i18nostream out;
}

#endif /* INTERN_I18N_HPP_ */
