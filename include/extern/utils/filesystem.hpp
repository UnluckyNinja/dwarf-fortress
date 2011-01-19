/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#ifndef EXTERN_UTILS_FILESYSTEM_HPP_
#define EXTERN_UTILS_FILESYSTEM_HPP_

#include "extern/string_vect.hpp"
#include "extern/svector.hpp"

/**
 * @{
 * @ingroup extern
 */
void find_files_by_pattern(char const*, stringvectst&);
void find_directories_by_pattern(char const*, stringvectst&);
void find_files_by_pattern_with_exception(char const*, svector< char* >&, char const*);
void find_directories_by_pattern_with_exception(char const*, stringvectst&, char const*);
/** @} */

#endif /* EXTERN_UTILS_FILESYSTEM_HPP_ */
