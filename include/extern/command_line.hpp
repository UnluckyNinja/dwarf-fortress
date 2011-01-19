/**
 * @file
 * @date 12 juin 2010
 * @todo comment
 */

#ifndef EXTERN_COMMAND_LINE_HPP_
#define EXTERN_COMMAND_LINE_HPP_

#include <string>

#include "extern/string_vect.hpp"

struct command_linest {
    ::std::string original;
    stringvectst arg_vect;

    long gen_id;
    unsigned long world_seed;
    char use_seed;
    ::std::string world_param;
    char use_param;

    /**
     * @ingroup extern
     */
    void init(const ::std::string &str);
    //    char grab_arg(string &source, long &pos);
    //    void handle_arg(string &arg);
    //
    command_linest();
};

#endif /* EXTERN_COMMAND_LINE_HPP_ */
