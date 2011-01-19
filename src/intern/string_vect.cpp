/**
 * @file
 * @date 17 juin 2010
 * @todo comment
 */

#include "extern/string_vect.hpp"

#include "extern/utils/compressor.hpp"

#include "intern/pstring.hpp"

void stringvectst::clean() {
  while (str.size() > 0) {
    delete str[0];
    str.erase(0);
  }
}

void stringvectst::add_string(const ::std::string &st) {
  pstringst *newp = new pstringst;
  newp->dat = st;
  str.push_back(newp);
}

long stringvectst::add_unique_string(const ::std::string &st) {
  long i;
  for (i = (long) str.size() - 1; i >= 0; i--) {
    if (str[i]->dat == st)
      return i;
  }
  add_string(st);
  return (long) str.size() - 1;
}

void stringvectst::add_string(const char *st) {
  if (st != 0) {
    pstringst *newp = new pstringst;
    newp->dat = st;
    str.push_back(newp);
  }
}

void stringvectst::insert_string(long k, const ::std::string &st) {
  pstringst *newp = new pstringst;
  newp->dat = st;
  if (static_cast< long > (str.size()) > k)
    str.insert(k, newp);
  else
    str.push_back(newp);
}

void stringvectst::read_file(file_compressorst &filecomp, long loadversion) {
  long dummy;
  filecomp.read_file(dummy);
  str.resize(dummy);

  long s;
  for (s = 0; s < dummy; s++) {
    str[s] = new pstringst;
    filecomp.read_file(str[s]->dat);
  }
}
void stringvectst::write_file(file_compressorst &filecomp) {
  long dummy = str.size();
  filecomp.write_file(dummy);

  long s;
  for (s = 0; s < dummy; s++) {
    filecomp.write_file(str[s]->dat);
  }
}

void stringvectst::copy_from(stringvectst &src) {
  clean();

  str.resize(src.str.size());

  long s;
  for (s = (long) src.str.size() - 1; s >= 0; s--) {
    str[s] = new pstringst;
    str[s]->dat = src.str[s]->dat;
  }
}

bool stringvectst::has_string(const ::std::string &st) {
  long i;
  for (i = (long) str.size() - 1; i >= 0; i--) {
    if (str[i]->dat == st)
      return true;
  }
  return false;
}

void stringvectst::remove_string(const ::std::string &st) {
  long i;
  for (i = (long) str.size() - 1; i >= 0; i--) {
    if (str[i]->dat == st) {
      delete str[i];
      str.erase(i);
    }
  }
}

void stringvectst::operator =(const ::std::vector< ::std::string >& vector) {
  ::std::vector< ::std::string >::const_iterator it = vector.begin();

  for (; it != vector.end(); ++it) {
    add_string(*it);
  }
}
