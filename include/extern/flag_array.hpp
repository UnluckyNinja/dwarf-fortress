/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_FLAG_ARRAY_HPP_
#define EXTERN_FLAG_ARRAY_HPP_

struct file_compressorst;

class flagarrayst {
  public:
    flagarrayst();
    ~flagarrayst();

    void set_size_on_flag_num(long flagnum);
    void set_size(long newsize);
    void clear_all();

    void copy_from(flagarrayst &src);

    /**
     * @ingroup hidden
     */
    bool has_flag(long checkflag);

    /**
     * @ingroup hidden
     */
    void add_flag(long checkflag);
    void toggle_flag(long checkflag);
    void remove_flag(long checkflag);

    void write_file(file_compressorst &filecomp);
    void read_file(file_compressorst &filecomp, long loadversion);

  private:
    unsigned char *array;
    long slotnum;
};

/**
 * @ingroup extern
 */
char get_slot_and_addbit_uchar(unsigned char &addbit, long &slot, long checkflag, long slotnum);

#endif /* EXTERN_FLAG_ARRAY_HPP_ */
