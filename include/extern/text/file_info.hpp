/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef EXTERN_TEXT_SYSTEM_FILE_INFO_HPP_
#define EXTERN_TEXT_SYSTEM_FILE_INFO_HPP_

#include <string>

struct text_infost;

class text_system_file_infost {
  public:
    long index;
    ::std::string filename;

    static text_system_file_infost* add_file_info(const ::std::string &newf, long newi, char newft);

    /**
     * @ingroup extern
     */
    void initialize_info();
    /**
     * @ingroup extern
     */
    void get_text(text_infost& text);
    void get_specific_text(text_infost& text, long num);

  protected:
    char file_token;
    long number;

    text_system_file_infost(const ::std::string &newf, long newi, char newft);
};

#endif /* EXTERN_TEXT_SYSTEM_FILE_INFO_HPP_ */
