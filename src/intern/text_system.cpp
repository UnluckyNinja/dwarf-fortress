/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#include "intern/text_system.hpp"

#include "extern/text/file_info.hpp"

void text_systemst::register_file_fixed(const ::std::string &file_name, int32_t index, char token, char initialize) {
  text_system_file_infost *tsfi = text_system_file_infost::add_file_info(file_name, index, token);
  if (initialize)
    tsfi->initialize_info();
  file_info.push_back(tsfi);
}

void text_systemst::register_file(const ::std::string &file_name, int32_t &index, char token, char initialize) {
  int32_t t;
  for (t = (int32_t) file_info.size() - 1; t >= 0; t--) {
    if (file_info[t]->filename == file_name) {
      //RESET CALLING INDEX AND BAIL IF THIS FILE IS ALREADY IN THE SYSTEM
      index = file_info[t]->index;
      return;
    }
  }

  text_system_file_infost *tsfi = text_system_file_infost::add_file_info(file_name, index, token);
  if (initialize)
    tsfi->initialize_info();
  file_info.push_back(tsfi);
}

void text_systemst::initialize_system() {
  int32_t t;
  for (t = (int32_t) file_info.size() - 1; t >= 0; t--)
    file_info[t]->initialize_info();
}

void text_systemst::get_text(int32_t index, text_infost &text) {
  int32_t t;
  for (t = (int32_t) file_info.size() - 1; t >= 0; t--) {
    if (file_info[t]->index == index) {
      file_info[t]->get_text(text);
      return;
    }
  }
}

void text_systemst::get_text(const ::std::string &file_name, text_infost &text) {
  int32_t t;
  for (t = (int32_t) file_info.size() - 1; t >= 0; t--) {
    if (file_info[t]->filename == file_name) {
      file_info[t]->get_text(text);
      return;
    }
  }
}

//void text_systemst::get_specific_text(int32_t index, text_infost &text, int32_t num) {
//  int32_t t;
//  for (t = (int32_t) file_info.size() - 1; t >= 0; t--) {
//    if (file_info[t]->index == index) {
//      file_info[t]->get_specific_text(text, num);
//      return;
//    }
//  }
//}

text_systemst::~text_systemst() {
  while (file_info.size() > 0) {
    delete file_info[0];
    file_info.erase(0);
  }
}
