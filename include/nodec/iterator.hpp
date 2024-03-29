#ifndef NODEC__ITERATOR_HPP_
#define NODEC__ITERATOR_HPP_

#include <cstddef>

namespace nodec {

template<class C>
constexpr auto size(const C &c) -> decltype(c.size()) {
    return c.size();
}

template<class T, std::size_t N>
constexpr std::size_t size(const T (&array)[N]) noexcept {
    return N;
}

} // namespace nodec

#endif