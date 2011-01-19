/**
 * @file
 * @date 12 juin 2010
 * @todo comment
 */

#ifndef EXTERN_ENABLER_INPUT_HPP_
#define EXTERN_ENABLER_INPUT_HPP_

#include <string>
#include "integer_types.hpp"

struct enabler_inputst {
    /**
     * @ingroup extern
     */
    ::std::string GetKeyDisplay(int32_t binding);

    /**
     * @ingroup extern
     */
    ::std::string GetBindingDisplay(int32_t binding);
};

#endif /* EXTERN_ENABLER_INPUT_HPP_ */
