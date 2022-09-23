#ifndef NODEC__UTILITY_HPP_
#define NODEC__UTILITY_HPP_

#include <type_traits>

namespace nodec {

/**
 * @brief Forms lvalue reference to const type of t.
 * Waiting for C++17.
 */
template<class T>
constexpr std::add_const_t<T> &as_const(T &t) noexcept {
    return t;
}

/**
 * @brief const rvalue reference overload is deleted to disallow rvalue arguments.
 */
template<class T>
void as_const(const T &&) = delete;

/**
 * @brief
 * Waiting for C++17.
 * @tparam T
 */
template<class T>
struct in_place_type_t {
    explicit in_place_type_t() = default;
};

template<class T>
constexpr in_place_type_t<T> in_place_type{};

struct in_place_t {
    explicit in_place_t() = default;
};

constexpr in_place_t in_place{};

} // namespace nodec

#endif