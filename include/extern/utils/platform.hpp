/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#ifndef EXTERN_UTILS_PLATFORM_HPP_
#define EXTERN_UTILS_PLATFORM_HPP_

/**
 * @{
 * @ingroup extern
 */
int MessageBox(int*, char const*, char const*, unsigned int);
unsigned long GetTickCount();
int CreateDirectory(char const*, void*);
char* itoa(int, char*, int);
/** @} */

#endif /* EXTERN_UTILS_PLATFORM_HPP_ */
