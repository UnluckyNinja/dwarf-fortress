/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_SVECTOR_HPP_
#define EXTERN_SVECTOR_HPP_

#include <vector>

#include <cstring>

template< class T, class A = ::std::allocator< T > >
class svector: public ::std::vector< T, A > {
  public:
#ifndef WIN32
    using std::vector< T, A >::begin;
    using std::vector< T, A >::end;
#endif

    void erase(typename std::vector< T, A >::size_type i) {
      std::vector< T, A > &vec = *this;
      vec.erase(begin() + i);
    }

    void insert(typename std::vector< T, A >::size_type i, const T &v) {
      std::vector< T, A > &vec = *this;
      vec.insert(begin() + i, v);
    }

    void operator=(const ::std::vector< ::std::string > &vector) {
      ::std::vector< ::std::string >::const_iterator it = vector.begin();

      for (; it != vector.end(); ++it) {
        // caller will have to free this
        char* chars = new char[it->length() + 1];

        memset(chars, 0, it->length() + 1);
        strcpy(chars, it->c_str());

        std::vector< T, A >::push_back(chars);
      }
    }
};

#endif /* EXTERN_SVECTOR_HPP_ */
