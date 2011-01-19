/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include <zlib.h>
#include <fstream>

#include "extern/interface.hpp"

#include "hidden/global.hpp"
#include "hidden/dwarf.hpp"
#include "extern/init.hpp"
#include "extern/graphics.hpp"
#include "extern/enabler.hpp"

#include "extern/music.hpp"
#include "extern/screen/movieplayer.hpp"

#include <logging/logging.hpp>

#include "intern/pstring.hpp"

#include "extern/utils/numbers.hpp"
#include "extern/utils/gamelog.hpp"

#include "byteswap.hpp"

struct cursesmovie_headerst {
    int dimx, dimy;
    int delayrate;
};

viewscreenst *interfacest::grab_lastscreen() {
  viewscreenst *currentscreen = &view;
  while (currentscreen->child)
    currentscreen = currentscreen->child;
  return currentscreen;
}

void interfacest::remove_to_first() {
  //GRAB LAST SCREEN AT THE END OF THE LIST
  viewscreenst *lastscreen = &view;
  while (lastscreen->child != NULL)
    lastscreen = lastscreen->child;

  //NO INTERFACE LEFT
  if (lastscreen == &view)
    return;

  //GO AHEAD
  while (lastscreen->parent != &view) {
    viewscreenst *par = lastscreen->parent;
    removescreen(lastscreen);
    lastscreen = par;
  }
}

void interfacest::removescreen(viewscreenst *scr) {
  //THE MINIMAP IS EXPENSIVE, SO WE REFRESH IT WHENEVER INTERFACE GETS IN THE WAY
  if (gamemode == GAMEMODE_DWARF)
    dwarf_remove_screen();

  //FIX LINKS
  if (scr->parent != NULL)
    scr->parent->child = scr->child;
  if (scr->child != NULL)
    scr->child->parent = scr->parent;

  //WASTE SCREEN
  delete scr;
}

char interfacest::is_supermovie_on() {
  return supermovie_on;
}

void interfacest::insertscreen_as_parent(viewscreenst *scr, viewscreenst *child) {
  if (child == NULL) {
    insertscreen_at_back(scr);
    return;
  }

  scr->child = child;
  scr->parent = child->parent;

  if (scr->parent != NULL)
    scr->parent->child = scr;
  child->parent = scr;
}

void interfacest::insertscreen_as_child(viewscreenst *scr, viewscreenst *parent) {
  if (parent == NULL) {
    insertscreen_at_back(scr);
    return;
  }

  scr->child = parent->child;
  scr->parent = parent;

  if (scr->child != NULL)
    scr->child->parent = scr;
  parent->child = scr;
}

void interfacest::insertscreen_at_back(viewscreenst *scr) {
  //GRAB CURRENT SCREEN AT THE END OF THE LIST
  viewscreenst *currentscreen = &view;
  while (currentscreen->child != NULL)
    currentscreen = currentscreen->child;

  //PUT IT ON TO THE BACK SCREEN
  insertscreen_as_child(scr, currentscreen);
}

void interfacest::insertscreen_at_front(viewscreenst *scr) {
  //PUT IT ON TO THE BASE
  insertscreen_as_child(scr, &view);
}

void interfacest::handlemovie(char flushall) {
  //SAVE A MOVIE FRAME INTO THE CURRENT MOVIE BUFFER
  if (supermovie_on == 1) {
    if (supermovie_delaystep > 0 && !flushall)
      supermovie_delaystep--;
    else {
      if (!flushall)
        supermovie_delaystep = supermovie_delayrate;

      if (!flushall || supermovie_delaystep == 0) {
        //SAVING CHARACTERS, THEN COLORS
        short x2, y2;
        for (x2 = 0; x2 < init.display.grid_x; x2++) {
          for (y2 = 0; y2 < init.display.grid_y; y2++) {
            supermoviebuffer[supermovie_pos] = gps.screen[x2 * gps.dimy * 4 + y2 * 4 + 0];

            supermovie_pos++;
          }
        }
        char frame_col;
        for (x2 = 0; x2 < init.display.grid_x; x2++) {
          for (y2 = 0; y2 < init.display.grid_y; y2++) {
            frame_col = gps.screen[x2 * gps.dimy * 4 + y2 * 4 + 1];
            frame_col |= (gps.screen[x2 * gps.dimy * 4 + y2 * 4 + 2] << 3);
            if (gps.screen[x2 * gps.dimy * 4 + y2 * 4 + 3])
              frame_col |= 64;
            supermoviebuffer[supermovie_pos] = frame_col;

            supermovie_pos++;
          }
        }
      }

      int frame_size = init.display.grid_x * init.display.grid_y * 2;
      if ((unsigned int) (supermovie_pos + frame_size) >= MOVIEBUFFSIZE || flushall) {
        int length = write_movie_chunk();

        //********************************** FUCK
        ::std::string str = "write: ";
        add_long_to_string(supermovie_pos, str);
        str += "/";
        add_long_to_string(length, str);
        errorlog_string(str);

        if (length > 5000000) {
          finish_movie();
        } else
          supermovie_pos = 0;
      }
    }
  }
}

void interfacest::finish_movie() {
  supermovie_on = 0;
  currentblocksize = 0;
  nextfilepos = 0;
  supermovie_pos = 0;
  viewscreen_movieplayerst::create(INTERFACE_PUSH_AT_BACK);
}

void interfacest::use_movie_input() {
  if (supermovie_on)
    handlemovie(1);
  finish_movie();
}

int interfacest::write_movie_chunk() {
  int inputsize = supermovie_pos;
  if ((unsigned int) inputsize > MOVIEBUFFSIZE)
    inputsize = MOVIEBUFFSIZE;

  //DUMP CURRENT BUFFER INTO A COMPRESSION STREAM
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

  c_stream.next_out = (Bytef*) supermoviebuffer_comp;
  c_stream.avail_out = COMPMOVIEBUFFSIZE;

  c_stream.next_in = (Bytef*) supermoviebuffer;
  c_stream.avail_in = inputsize;

  while (c_stream.total_in != (unsigned int) inputsize && (unsigned int) c_stream.total_out < COMPMOVIEBUFFSIZE) {
    //c_stream.avail_in = c_stream.avail_out = 1; // force small buffers
    err = deflate(&c_stream, Z_NO_FLUSH);
    if (err != Z_OK) {
      __error
        << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
      exit(1);
    }
  }

  // Finish the stream, still forcing small buffers:
  for (;;) {
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

  int length = 0;

  if (c_stream.total_out > 0) {
    if (first_movie_write) {
      //GET RID OF ANY EXISTING MOVIES IF THIS IS THE FIRST TIME THROUGH
      unlink(movie_file.c_str());
    }

    //OPEN UP THE MOVIE FILE AND APPEND
    ::std::fstream f;
    f.open(movie_file.c_str(), ::std::fstream::out | ::std::fstream::binary | ::std::fstream::app);

    if (f.is_open()) {
      //WRITE A HEADER
      if (first_movie_write) {
        int swp_l = byteswap(movie_version);
        f.write((const char *) &swp_l, sizeof(int));

        cursesmovie_headerst cmh;
        cmh.dimx = init.display.grid_x;
        cmh.dimy = init.display.grid_y;
        cmh.delayrate = supermovie_delayrate;
        cmh.dimx = byteswap(cmh.dimx);
        cmh.dimy = byteswap(cmh.dimy);
        cmh.delayrate = byteswap(cmh.delayrate);
        f.write((const char *) &cmh, sizeof(cursesmovie_headerst));

        int s = byteswap(gview.supermovie_sound.str.size());
        f.write((const char *) &s, sizeof(int));
        char buf[50];
        for (::std::size_t i = 0; i < gview.supermovie_sound.str.size(); i++) {
          strcpy(buf, gview.supermovie_sound.str[i]->dat.c_str());
          f.write(buf, sizeof(char) * 50);
        }

        int i1, i2;
        for (i1 = 0; i1 < 200; i1++) {
          for (i2 = 0; i2 < SOUND_CHANNELNUM; i2++) {
#ifndef NO_FMOD
            swp_l = byteswap(gview.supermovie_sound_time[i1][i2]);
#else
            swp_l=-1;
#endif
            f.write((const char *) &swp_l, sizeof(int));
          }
        }
      }

      //WRITE IT
      int compsize = byteswap(c_stream.total_out);
      f.write((const char *) &compsize, sizeof(int));
      f.write((const char *) supermoviebuffer_comp, c_stream.total_out);

      f.seekg(0, ::std::ios::beg);
      int beg = f.tellg();
      f.seekg(0, ::std::ios::end);
      int end = f.tellg();
      length = end - beg;

      f.close();
    } else
      supermovie_on = 0;

    first_movie_write = 0;
  }

  return length;
}

void interfacest::read_movie_chunk(int &maxmoviepos, char &is_playing) {
  //OPEN UP THE MOVIE FILE AND MOVE TO CORRECT POSITION
  ::std::fstream f;
  f.open(movie_file.c_str(), ::std::fstream::in | ::std::fstream::binary);

  if (f.is_open()) {
    f.seekg(0, ::std::ios::beg);
    int beg = f.tellg();
    f.seekg(0, ::std::ios::end);
    int end = f.tellg();
    int file_size = end - beg;

    if (gview.nextfilepos < file_size) {
      f.seekg(gview.nextfilepos, ::std::ios::beg);

      //LOAD THE HEADER
      char fail = 0;
      if (gview.nextfilepos == 0) {
        int loadversion;
        f.read((char *) &loadversion, sizeof(int));
        loadversion = byteswap(loadversion);

        if (loadversion > movie_version)
          fail = 1;

        cursesmovie_headerst cmh;
        f.read((char *) &cmh, sizeof(cursesmovie_headerst));
        cmh.dimx = byteswap(cmh.dimx);
        cmh.dimy = byteswap(cmh.dimy);
        cmh.delayrate = byteswap(cmh.delayrate);

        enabler.override_grid_size(cmh.dimx, cmh.dimy);
        if (!gview.original_fps)
          gview.original_fps = enabler.get_fps();
        enabler.set_fps(100);

        gview.supermovie_delayrate = cmh.delayrate;
        gview.supermovie_delaystep = cmh.delayrate;

        gview.supermovie_sound.clean();
        if (loadversion >= 10001) {
          int num;
          f.read((char *) &num, sizeof(int));
          num = byteswap(num);
          gview.nextfilepos += sizeof(int);
          char buf[50];
          int s;
          for (s = 0; s < num; s++) {
            f.read(buf, sizeof(char) * 50);
            ::std::string str = buf;
            gview.supermovie_sound.add_string(str);
            gview.nextfilepos += sizeof(char) * 50;
          }

          int i1, i2, swp_l;
          for (i1 = 0; i1 < 200; i1++) {
            for (i2 = 0; i2 < SOUND_CHANNELNUM; i2++) {
              f.read((char *) &swp_l, sizeof(int));
#ifndef NO_FMOD
              gview.supermovie_sound_time[i1][i2] = byteswap(swp_l);
#endif
            }
          }

          gview.nextfilepos += sizeof(int) * 200 * SOUND_CHANNELNUM;
        } else {
#ifndef NO_FMOD
          int i, c;
          for (i = 0; i < 200; i++) {
            for (c = 0; c < SOUND_CHANNELNUM; c++)
              gview.supermovie_sound_time[i][c] = -1;
          }
#endif
        }

        gview.nextfilepos += sizeof(int) + sizeof(cursesmovie_headerst);

#ifndef NO_FMOD
        //HANDLE SOUND LOADING
        for (::std::size_t s = 0; s < gview.supermovie_sound.str.size(); s++) {
          ::std::string filename = "data/sound/";
          filename += gview.supermovie_sound.str[s]->dat;
          filename += ".ogg";

          musicsound.set_sound(filename, s);
        }
#endif
      }

      if (!fail) {
        //READ IT
        f.read((char *) &gview.currentblocksize, sizeof(int));
        gview.currentblocksize = byteswap(gview.currentblocksize);
        f.read((char *) gview.supermoviebuffer_comp, gview.currentblocksize);

        gview.nextfilepos += gview.currentblocksize + sizeof(int);

        //UNCOMPRESS IT
        z_stream d_stream; // decompression stream

        d_stream.zalloc = (alloc_func) 0;
        d_stream.zfree = (free_func) 0;
        d_stream.opaque = (voidpf) 0;

        d_stream.next_in = (Bytef*) gview.supermoviebuffer_comp;
        d_stream.avail_in = gview.currentblocksize;

        int err = inflateInit(&d_stream);
        if (err != Z_OK) {
          __error
            << "One of the compressed files on disk has errors in it.  Restore from backup if you are able.";
          exit(1);
        }

        d_stream.next_out = gview.supermoviebuffer;
        d_stream.avail_out = MOVIEBUFFSIZE;

        while ((unsigned int) d_stream.total_out < MOVIEBUFFSIZE && d_stream.total_in < (unsigned int) gview.currentblocksize) {
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

        gview.supermovie_pos = 0;
        maxmoviepos = d_stream.total_out;
      } else {
        is_playing = 0;
        enabler.release_grid_size();
        if (gview.original_fps)
          enabler.set_fps(gview.original_fps);
      }
    } else {
      is_playing = 0;
      enabler.release_grid_size();
      if (gview.original_fps)
        enabler.set_fps(gview.original_fps);
    }

    f.close();
  } else {
    is_playing = 0;
    enabler.release_grid_size();
    if (gview.original_fps)
      enabler.set_fps(gview.original_fps);
  }
}
