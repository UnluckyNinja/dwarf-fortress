/**
 * @file
 * @date 26 juin 2010
 * @todo comment
 */

#ifndef EXTERN_STRING_VECT_HPP_
#define EXTERN_STRING_VECT_HPP_

#include <string>

#include "extern/svector.hpp"

struct file_compressorst;
struct pstringst;

class stringvectst {
  public:
    svector< pstringst* > str;

    void add_string(const ::std::string &st);
    long add_unique_string(const ::std::string &st);
    void add_string(const char *st);
    void insert_string(long k, const ::std::string &st);

    /**
     * @ingroup hidden
     */
    ~stringvectst();

    void clean();

    void read_file(file_compressorst &filecomp, long loadversion);
    void write_file(file_compressorst &filecomp);

    void copy_from(stringvectst &src);

    bool has_string(const ::std::string &st);
    void remove_string(const ::std::string &st);

    void operator=(stringvectst &two);

    void operator=(const ::std::vector< ::std::string > &vector);
};

#endif /* EXTERN_STRING_VECT_HPP_ */
