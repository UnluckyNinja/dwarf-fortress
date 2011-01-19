/**
 * @file
 * @date 18 janv. 2011
 * @todo comment
 */

#ifndef META_EITHER_HPP_
#define META_EITHER_HPP_

template< typename L, typename R >
struct Either {
    bool isL;
    union {
        L left;
        R right;
    };
    Either(const L &l) {
      isL = true;
      left = l;
    }
    Either(const R &r) {
      isL = false;
      right = r;
    }
};

#endif /* META_EITHER_HPP_ */
