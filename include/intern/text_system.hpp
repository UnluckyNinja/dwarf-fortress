/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXT_SYSTEM_HPP_
#define INTERN_TEXT_SYSTEM_HPP_

#include <string>
#include "integer_types.hpp"

struct text_system_file_infost;
struct text_infost;

#include "extern/svector.hpp"

class text_systemst {
  public:
    void register_file_fixed(const ::std::string& file_name, int32_t index, char token, char initialize);
    void register_file(const ::std::string& file_name, int32_t &index, char token, char initialize);
    void initialize_system();
    void get_text(int32_t index, text_infost& text);
    void get_text(const ::std::string& file_name, text_infost& text);
//    void get_specific_text(int32_t index, text_infost& text, int32_t num);

    ~text_systemst();

  protected:
    svector< text_system_file_infost* > file_info;
};

#endif /* INTERN_TEXT_SYSTEM_HPP_ */
