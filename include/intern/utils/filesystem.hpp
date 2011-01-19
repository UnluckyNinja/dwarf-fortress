/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#ifndef INTERN_UTILS_FILESYSTEM_HPP_
#define INTERN_UTILS_FILESYSTEM_HPP_

#include <string>
#include <vector>

void find_files_by_regexp(const ::std::string&, ::std::vector< ::std::string >&, const ::std::string& = "^$");
void find_folders_by_regexp(const ::std::string&, ::std::vector< ::std::string >&, const ::std::string& = "^$");

void find_files_by_glob(const ::std::string&, ::std::vector< ::std::string >&, const ::std::string& = "");
void find_folders_by_glob(const ::std::string&, ::std::vector< ::std::string >&, const ::std::string& = "");

void copy_file(const ::std::string& source, const ::std::string& dest);
void replace_file(const ::std::string& source, const ::std::string& dest);

#endif /* INTERN_UTILS_FILESYSTEM_HPP_ */
