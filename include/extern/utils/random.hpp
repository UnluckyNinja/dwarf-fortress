/**
 * @file
 * @date 9 juin 2010
 * @todo comment
 */

#ifndef EXTERN_UTILS_RANDOM_HPP_
#define EXTERN_UTILS_RANDOM_HPP_

#include "integer_types.hpp"

/**
 * @{
 * @ingroup extern
 */
uint32_t mt_trandom();
int32_t basic_random(int32_t = 2147483647);
void pop_trandom_uniform_seed();
void push_trandom_triple_seed(uint32_t, uint32_t, uint32_t);
void push_trandom_uniform_seed(uint32_t);
void r_num();
void mt_init();
/** @} */

#endif /* EXTERN_UTILS_RANDOM_HPP_ */
