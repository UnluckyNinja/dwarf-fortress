/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/utils/compressor.hpp"

#include "byteswap.hpp"

#include <logging/logging.hpp>

#include <zlib.h>

char file_compressorst::def_ibuff[FILE_IN_BUFF];
char file_compressorst::def_obuff[FILE_OUT_BUFF];

void file_compressorst::write_file_fixedlength(char *var) {
  short len = strlen(var);
  write_file(len);
  if (len > 0)
    write_file(var, len * sizeof(char));
}

void file_compressorst::read_file_fixedlength(char *var) {
  short len;
  read_file(len);//DO NOT NEED TO ALLOCATE SPACE
  if (len > 0)
    read_file(var, len * sizeof(char));
  var[len] = '\x0';
}

char file_compressorst::write_file(long var) {
  var = byteswap(var);
  return write_file(&var, sizeof(long));
}

char file_compressorst::read_file(long &var) {
  char ret = read_file(&var, sizeof(long));
  var = byteswap(var);
  return ret;
}

char file_compressorst::write_file(int var) {
  var = byteswap(var);
  return write_file(&var, sizeof(int));
}

char file_compressorst::read_file(int &var) {
  char ret = read_file(&var, sizeof(int));
  var = byteswap(var);
  return ret;
}

char file_compressorst::write_file(short var) {
  var = byteswap(var);
  return write_file(&var, sizeof(short));
}

char file_compressorst::read_file(short &var) {
  char ret = read_file(&var, sizeof(short));
  var = byteswap(var);
  return ret;
}

char file_compressorst::write_file(char var) {
  return write_file(&var, sizeof(char));
}

char file_compressorst::read_file(char &var) {
  return read_file(&var, sizeof(char));
}

char file_compressorst::write_file(bool var) {
  char temp;
  if (var)
    temp = 1;
  else
    temp = 0;
  return write_file(&temp, sizeof(char));
}

char file_compressorst::read_file(bool &var) {
  char temp;
  if (!read_file(&temp, sizeof(char)))
    return 0;
  var = (temp != 0);
  return 1;
}

void file_compressorst::write_file(svector< bool > &vect) {
  long s = vect.size();
  write_file(s);
  bool bl;//JUST FOR PARITY WITH read BELOW
  long i;
  for (i = 0; i < s; i++) {
    bl = vect[i];
    write_file(bl);
  }
}

void file_compressorst::read_file(svector< bool > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  bool bl;//NO IDEA WHY IT CAN'T JUST TAKE vect[i]
  long i;
  for (i = 0; i < s; i++) {
    read_file(bl);
    vect[i] = bl;
  }
}

char file_compressorst::write_file(unsigned long var) {
  var = byteswap(var);
  return write_file(&var, sizeof(unsigned long));
}

char file_compressorst::read_file(unsigned long &var) {
  char ret = read_file(&var, sizeof(unsigned long));
  var = byteswap(var);
  return ret;
}

char file_compressorst::write_file(unsigned int var) {
  var = byteswap(var);
  return write_file(&var, sizeof(unsigned int));
}

char file_compressorst::read_file(unsigned int &var) {
  char ret = read_file(&var, sizeof(unsigned int));
  var = byteswap(var);
  return ret;
}

char file_compressorst::write_file(unsigned short var) {
  var = byteswap(var);
  return write_file(&var, sizeof(unsigned short));
}

char file_compressorst::read_file(unsigned short &var) {
  char ret = read_file(&var, sizeof(unsigned short));
  var = byteswap(var);
  return ret;
}

char file_compressorst::write_file(unsigned char var) {
  return write_file(&var, sizeof(unsigned char));
}

char file_compressorst::read_file(unsigned char &var) {
  return read_file(&var, sizeof(unsigned char));
}

void file_compressorst::write_file(svector< short > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< short > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< unsigned short > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< unsigned short > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< unsigned int > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< unsigned int > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< int > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< int > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< unsigned long > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< unsigned long > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< unsigned char > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< unsigned char > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< char > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< char > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

void file_compressorst::write_file(svector< long > &vect) {
  long s = vect.size();
  write_file(s);
  long i;
  for (i = 0; i < s; i++) {
    write_file(vect[i]);
  }
}

void file_compressorst::read_file(svector< long > &vect) {
  long s;
  read_file(s);
  vect.resize(s);
  long i;
  for (i = 0; i < s; i++) {
    read_file(vect[i]);
  }
}

char file_compressorst::load_new_in_buffer() {
  if (!f.is_open())
    return 0;
  if (!compressed)
    return 1;

  in_buffer_position = 0;
  in_buffer_amount_loaded = 0;

  //LOAD THE BLOCK OF COMPRESSED DATA
  f.read((char*) &out_buffer_amount_written, sizeof(long));
  out_buffer_amount_written = byteswap(out_buffer_amount_written);
  f.read(out_buffer, out_buffer_amount_written);

  int err;

  if (out_buffer_amount_written > 0) {
    //TARN: adapted from zlib example files

    //UNCOMPRESS
    z_stream d_stream; // decompression stream

    d_stream.zalloc = (alloc_func) 0;
    d_stream.zfree = (free_func) 0;
    d_stream.opaque = (voidpf) 0;

    d_stream.next_in = (Bytef*) out_buffer;
    d_stream.avail_in = out_buffer_amount_written;

    err = inflateInit(&d_stream);
    if (err != Z_OK) {
      __error
        << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
      exit(1);
    }

    d_stream.next_out = (Bytef*) in_buffer;
    d_stream.avail_out = in_buffersize;

    while (d_stream.total_out < in_buffersize && d_stream.total_in < out_buffer_amount_written) {
      //d_stream.avail_in = d_stream.avail_out = 1; // force small buffers
      err = inflate(&d_stream, Z_NO_FLUSH);

      if (err == Z_STREAM_END)
        break;
      if (err != Z_OK) {
        __error
          << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
        exit(1);
      }
    }

    err = inflateEnd(&d_stream);
    if (err != Z_OK) {
      __error
        << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
      exit(1);
    }

    in_buffer_amount_loaded = d_stream.total_out;

    return 1;
  } else
    return 0;
}

file_compressorst::file_compressorst() {
  compressed = true;

  in_buffer = def_ibuff;
  in_buffersize = FILE_IN_BUFF;
  in_buffer_amount_loaded = 0;
  in_buffer_position = 0;

  out_buffer = def_obuff;
  out_buffersize = FILE_OUT_BUFF;
  out_buffer_amount_written = 0;

  f.clear();
}

file_compressorst::~file_compressorst() {
  close_file();
}
