/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include <memory>

#include "extern/flag_array.hpp"

#include "extern/utils/compressor.hpp"

flagarrayst::flagarrayst() {
  slotnum = 0;
  array = 0;
}

flagarrayst::~flagarrayst() {
  if (array != 0) {
    delete[] array;
  }

  array = 0;
  slotnum = 0;
}

void flagarrayst::set_size_on_flag_num(long flagnum) {
  if (flagnum <= 0) {
    return;
  }

  set_size(((flagnum - 1) >> 3) + 1);
}

void flagarrayst::set_size(long newsize) {
  if (newsize <= 0)
    return;

  if (array != 0) {
    delete[] array;
  }

  array = new unsigned char[newsize];
  memset(array, 0, sizeof(unsigned char) * newsize);

  slotnum = newsize;
}

void flagarrayst::clear_all() {
  if (slotnum <= 0)
    return;

  if (array != 0) {
    memset(array, 0, sizeof(unsigned char) * slotnum);
  }
}

void flagarrayst::copy_from(flagarrayst &src) {
  clear_all();

  if (src.slotnum > 0) {
    set_size(src.slotnum);
    memmove(array, src.array, sizeof(unsigned char) * slotnum);
  }
}

void flagarrayst::toggle_flag(long checkflag) {
  long slot;
  unsigned char addbit;
  if (get_slot_and_addbit_uchar(addbit, slot, checkflag, slotnum)) {
    array[slot] ^= addbit;
  }
}

void flagarrayst::remove_flag(long checkflag) {
  long slot;
  unsigned char addbit;
  if (get_slot_and_addbit_uchar(addbit, slot, checkflag, slotnum)) {
    array[slot] &= ~addbit;
  }
}

void flagarrayst::write_file(file_compressorst &filecomp) {
  filecomp.write_file(slotnum);
  if (slotnum > 0) {
    long ind;
    for (ind = 0; ind < slotnum; ind++)
      filecomp.write_file(array[ind]);
  }
}

void flagarrayst::read_file(file_compressorst &filecomp, long loadversion) {
  long newsl;
  filecomp.read_file(newsl);
  if (newsl > 0) {
    //AVOID UNNECESSARY DELETE/NEW
    if (array != 0 && slotnum != newsl) {
      delete[] array;
      array = new unsigned char[newsl];
    }
    if (array == 0)
      array = new unsigned char[newsl];

    long ind;
    for (ind = 0; ind < newsl; ind++)
      filecomp.read_file(array[ind]);

    slotnum = newsl;
  } else if (array != 0) {
    delete[] array;
    array = 0;
  }
}

char get_slot_and_addbit_uchar(unsigned char &addbit, long &slot, long checkflag, long slotnum) {
  if (checkflag < 0)
    return 0;

  //FIND PROPER SLOT
  slot = checkflag >> 3;
  if (slot >= slotnum)
    return 0;

  //FIND PROPER BIT IN THAT SLOT
  addbit = 1 << (checkflag % 8);

  return 1;
}

bool flagarrayst::has_flag(long checkflag) {
  long slot;
  unsigned char addbit;
  if (get_slot_and_addbit_uchar(addbit, slot, checkflag, slotnum)) {
    return (array[slot] & addbit) != 0;
  } else
    return false;
}

void flagarrayst::add_flag(long checkflag) {
  long slot;
  unsigned char addbit;
  if (get_slot_and_addbit_uchar(addbit, slot, checkflag, slotnum)) {
    array[slot] |= addbit;
  }
}

