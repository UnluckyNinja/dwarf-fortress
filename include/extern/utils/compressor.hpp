/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_UTILS_COMPRESSOR_HPP_
#define EXTERN_UTILS_COMPRESSOR_HPP_

#include <string>
#include <fstream>

#include "extern/svector.hpp"

#define FILE_IN_BUFF 1000000
#define FILE_OUT_BUFF 2000000

class file_compressorst {
  public:
    bool compressed;
    ::std::fstream f;

    /**
     * @ingroup extern
     */
    char open_file(const ::std::string& filename, char existing_only);
    /**
     * @ingroup extern
     */
    void close_file();
    /**
     * @ingroup extern
     */
    char write_file(::std::string &str);
    /**
     * @ingroup extern
     */
    char read_file(::std::string &str);

    /**
     * @ingroup extern
     */
    char load_posnull_pointer();
    /**
     * @ingroup extern
     */
    char save_posnull_pointer(void *ptr);

    void write_file_fixedlength(char *var);
    void read_file_fixedlength(char *var);
    char write_file(long var);
    char read_file(long &var);
    char write_file(int var);
    char read_file(int &var);
    char write_file(short var);
    char read_file(short &var);
    char write_file(char var);
    char read_file(char &var);
    char write_file(bool var);
    char read_file(bool &var);
    void write_file(svector< bool > &vect);
    void read_file(svector< bool > &vect);
    char write_file(unsigned long var);
    char read_file(unsigned long &var);
    char write_file(unsigned int var);
    char read_file(unsigned int &var);
    char write_file(unsigned short var);
    char read_file(unsigned short &var);
    char write_file(unsigned char var);
    char read_file(unsigned char &var);
    void write_file(svector< short > &vect);
    void read_file(svector< short > &vect);
    void write_file(svector< unsigned short > &vect);
    void read_file(svector< unsigned short > &vect);
    void write_file(svector< unsigned int > &vect);
    void read_file(svector< unsigned int > &vect);
    void write_file(svector< int > &vect);
    void read_file(svector< int > &vect);
    void write_file(svector< unsigned long > &vect);
    void read_file(svector< unsigned long > &vect);
    void write_file(svector< unsigned char > &vect);
    void read_file(svector< unsigned char > &vect);
    void write_file(svector< char > &vect);
    void read_file(svector< char > &vect);
    void write_file(svector< long > &vect);
    void read_file(svector< long > &vect);

    char load_new_in_buffer();
    /**
     * @ingroup extern
     */
    char flush_in_buffer();

    file_compressorst();
    /**
     * @ingroup extern
     */
    file_compressorst(char *new_in_buffer, long new_in_buffersize, char *new_out_buffer, long new_out_buffersize);
    ~file_compressorst();

  private:
    /**
     * @ingroup extern
     */
    char write_file(void *write_var, long write_size);
    /**
     * @ingroup extern
     */
    char read_file(void *read_var, long read_size);

    char *in_buffer;
    long in_buffersize;
    long in_buffer_amount_loaded;
    long in_buffer_position;

    char *out_buffer;
    long out_buffersize;
    long out_buffer_amount_written;

    static char def_ibuff[FILE_IN_BUFF];
    static char def_obuff[FILE_OUT_BUFF];
};

#endif /* EXTERN_UTILS_COMPRESSOR_HPP_ */
