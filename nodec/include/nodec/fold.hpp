#ifndef NODEC__FOLD_HPP_
#define NODEC__FOLD_HPP_

#include <functional>

namespace nodec {
/*
 *   Note:
 *       * <https://stackoverflow.com/questions/51005897/what-is-a-good-alternative-to-this-c17-fold-expression-in-c14>
 */

template<class F, class A0>
auto fold(F &&f, A0 &&a0) {
    return std::forward<A0>(a0);
}

template<class F, class A0, class... As>
auto fold(F &&f, A0 &&a0, As &&...as) {
    return f(std::forward<A0>(a0), fold(f, std::forward<As>(as)...));
}

} // namespace nodec

#endif