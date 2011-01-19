/**
 * @file
 * @date 27 juin 2010
 * @todo comment
 */

#ifndef INTERN_TEXTURE_FULL_ID_HPP_
#define INTERN_TEXTURE_FULL_ID_HPP_

struct texture_fullid {
    int texpos;
    float r, g, b;
    float br, bg, bb;

    bool operator<(const texture_fullid &other) const;
};

#endif /* INTERN_TEXTURE_FULL_ID_HPP_ */
