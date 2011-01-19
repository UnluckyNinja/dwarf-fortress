/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "extern/utils/compressor.hpp"

#include "byteswap.hpp"

#include <logging/logging.hpp>

#include <zlib.h>

char file_compressorst::open_file(const ::std::string& filename, char existing_only) {
  if (filename.empty())
    return 0;

  //RESET DATA
  in_buffer_amount_loaded = 0;
  in_buffer_position = 0;
  out_buffer_amount_written = 0;

  /*
   //CHECK IF FILE ALREADY EXISTS
   f.open(filename.c_str(), ::std::fstream::in);
   char file_exists = f.is_open();
   if (file_exists)f.close();

   //OPEN FILE
   if(!existing_only || file_exists)
   f.open(filename.c_str(), ::std::fstream::in | ::std::fstream::out | ::std::fstream::binary);
   else
   f.open(filename.c_str(), ::std::fstream::in | ::std::fstream::out | ::std::fstream::binary);
   */
  //if(existing_only)h=CreateFile(filename.c_str(),GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
  //else h=CreateFile(filename.c_str(),GENERIC_READ|GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

  if (f.is_open())
    f.close();
  f.clear();

  if (existing_only)
    f.open(filename.c_str(), ::std::fstream::in | ::std::fstream::out | ::std::fstream::binary);
  else
    f.open(filename.c_str(), ::std::fstream::in | ::std::fstream::out | ::std::fstream::binary | ::std::fstream::trunc);

  if (f.is_open())
    return 1;
  else
    return 0;
}

void file_compressorst::close_file() {
  if (f.is_open()) {
    f.close();
  }
}

char file_compressorst::write_file(::std::string& str) {
  short ln = str.length();
  if (ln >= 10000 || ln < 0)
    ln = 0;

  if (!write_file(ln))
    return 0;

  if (ln == 0)
    return 1;

  if (!write_file((void *) str.c_str(), sizeof(char) * ln))
    return 0;

  return 1;
}

char file_compressorst::read_file(::std::string& str) {
  str.erase();

  short ln;

  if (!read_file(ln))
    return 0;

  if (ln == 0)
    return 1;

  char *strar = new char[ln + 1];

  if (!read_file(strar, sizeof(char) * ln)) {
    delete[] strar;
    return 0;
  }
  strar[ln] = '\x0';

  str = strar;

  delete[] strar;
  return 1;
}

char file_compressorst::load_posnull_pointer() {
  char dummy;
  read_file(dummy);

  if (!dummy)
    return 0;
  else
    return 1;
}

char file_compressorst::save_posnull_pointer(void *ptr) {
  char dummy;

  if (ptr == NULL) {
    dummy = 0;
    write_file(dummy);
    return 0;
  } else {
    dummy = 1;
    write_file(dummy);
    return 1;
  }
}

char file_compressorst::flush_in_buffer() {
  if (!f.is_open())
    return 0;
  if (!compressed)
    return 1;
  if (in_buffer_amount_loaded == 0)
    return 1;//EXTRA CALLS TO FLUSH SHOULDN'T KILL YOU

  //TARN: adapted from zlib example files

  //SET UP THE COMPRESSOR
  z_stream c_stream;
  int err;

  c_stream.zalloc = (alloc_func) 0;
  c_stream.zfree = (free_func) 0;
  c_stream.opaque = (voidpf) 0;

  err = deflateInit(&c_stream, 9);
  if (err != Z_OK) {
    __error
      << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
    exit(1);
  }

  c_stream.next_out = (Bytef*) out_buffer;
  c_stream.avail_out = out_buffersize;

  //SEND IT TO THE COMPRESSOR
  c_stream.next_in = (Bytef*) in_buffer;
  c_stream.avail_in = in_buffer_amount_loaded;

  while (c_stream.total_in != in_buffer_amount_loaded && c_stream.total_out < out_buffersize) {
    err = deflate(&c_stream, Z_NO_FLUSH);
    if (err != Z_OK) {
      __error
        << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
      exit(1);
    }
  }

  //FINISH UP THE STREAM
  while (1) {
    err = deflate(&c_stream, Z_FINISH);
    if (err == Z_STREAM_END)
      break;
    if (err != Z_OK) {
      __error
        << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
      exit(1);
    }
  }

  err = deflateEnd(&c_stream);
  if (err != Z_OK) {
    __error
      << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
    exit(1);
  }

  //SAVE THE COMPRESSED BIT AS A GIANT BLOCK
  if (c_stream.total_out > 0) {
    long compsize = byteswap(long(c_stream.total_out));

    //WRITE IT
    f.write((char*) &compsize, sizeof(long));
    f.write(out_buffer, c_stream.total_out);
  }

  in_buffer_position = 0;
  in_buffer_amount_loaded = 0;

  return 1;
}

file_compressorst::file_compressorst(char *new_in_buffer, long new_in_buffersize, char *new_out_buffer,
    long new_out_buffersize) {
  compressed = true;

  in_buffer = new_in_buffer;
  in_buffersize = new_in_buffersize;
  in_buffer_amount_loaded = 0;
  in_buffer_position = 0;

  out_buffer = new_out_buffer;
  out_buffersize = new_out_buffersize;
  out_buffer_amount_written = 0;

  f.clear();
}

char file_compressorst::write_file(void *write_var, long write_size) {
  if (!f.is_open())
    return 0;

  if (!compressed) {
    f.write((char *) write_var, write_size);
    return 1;
  }

  //WRITE OUT THE VARIABLE CHUNK BY CHUNK
  while (write_size > 0) {
    //FLUSH THE BUFFER IF NECESSARY
    if (in_buffer_amount_loaded >= in_buffersize) {
      if (!flush_in_buffer())
        return 0;
    }

    //SET THE AMOUNT TO COPY
    long copy_size = in_buffersize - in_buffer_amount_loaded;
    if (write_size < copy_size)
      copy_size = write_size;

    //COPY THE NEXT CHUNK INTO THE BUFFER
    memmove(in_buffer + in_buffer_amount_loaded, write_var, copy_size);

    write_var = ((char *) write_var) + copy_size;
    write_size -= copy_size;
    in_buffer_amount_loaded += copy_size;
  }

  return 1;
}

char file_compressorst::read_file(void *read_var, long read_size) {
  if (!f.is_open())
    return 0;

  if (!compressed) {
    f.read((char *) read_var, read_size);
    return 1;
  }

  //NOW LOAD INTO read_var UNTIL DONE
  while (read_size > 0) {
    //GET A BUFFER IF NECESSARY
    if (in_buffer_amount_loaded == 0 || in_buffer_position >= in_buffer_amount_loaded) {
      if (!load_new_in_buffer())
        return 0;
    }

    //BAIL IF STILL NO BUFFER LEFT
    if (in_buffer_amount_loaded == 0)
      return 0;

    //SET THE AMOUNT TO COPY
    long copy_size = in_buffer_amount_loaded - in_buffer_position;
    if (read_size < copy_size)
      copy_size = read_size;

    //COPY
    memmove(read_var, in_buffer + in_buffer_position, copy_size);

    read_var = ((char *) read_var) + copy_size;
    read_size -= copy_size;
    in_buffer_position += copy_size;
  }

  return 1;
}
