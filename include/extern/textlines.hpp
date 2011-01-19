/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXTLINES_HPP_
#define INTERN_TEXTLINES_HPP_

#include "extern/string_vect.hpp"

struct textlinesst {
    stringvectst text;

    /**
     * @ingroup extern
     */
    void load_raw_to_lines(char *filename);

    void clean_lines();
    void copy_from(textlinesst &ot);
};

#endif /* INTERN_TEXTLINES_HPP_ */
