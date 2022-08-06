#ifndef NODEC__TYPE_TRAITS_HPP_
#define NODEC__TYPE_TRAITS_HPP_

#include <iostream>
#include <type_traits>

namespace nodec {

/*
    Note:
        * <https://www.reddit.com/r/cpp/comments/fblqwd/is_printable_check_if_a_value_is_printable_at/>

*/

template<typename T, typename = int>
struct is_printable : std::false_type {};

template<typename T>
struct is_printable<T, decltype(std::cout << std::declval<T>(), 0)> : std::true_type {};

} // namespace nodec

#endif