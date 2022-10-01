#ifndef NODEC__ALGORITHM_HPP_
#define NODEC__ALGORITHM_HPP_

namespace nodec {

/**
 * @brief Clamp v in range [low, high].
 *
 * @tparam T
 * @param v
 * @param low
 * @param high
 * @return constexpr const T&
 */
template<class T>
constexpr const T &clamp(const T &v, const T &low, const T &high) {
    return v < low ? low : (high < v ? high : v);
}

} // namespace nodec

#endif