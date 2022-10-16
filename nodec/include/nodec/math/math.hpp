#ifndef NODEC__MATH__MATH_HPP_
#define NODEC__MATH__MATH_HPP_

#include "../matrix4x4.hpp"
#include "../quaternion.hpp"
#include "../utility.hpp"
#include "../vector2.hpp"
#include "../vector3.hpp"
#include "../vector4.hpp"

#include <cmath>
#include <limits>
#include <type_traits>

namespace nodec {
namespace math {

/**
 * @note
 *   <https://cpprefjp.github.io/lang/cpp14/variable_templates.html>
 */
template<class T>
constexpr T pi = static_cast<T>(3.14159265358979323846);

template<class T>
constexpr T deg2rad = pi<T> / static_cast<T>(180.0);

template<class T>
constexpr T rad2deg = static_cast<T>(180.0) / pi<T>;

template<typename T>
inline Quaternion<T> conj(const Quaternion<T> &q) {
    return Quaternion<T>(-q.x, -q.y, -q.z, q.w);
}

/**
 * @note
 *   <https://en.wikipedia.org/wiki/Norm_(mathematics)>
 */
template<typename T>
inline T norm(const Vector2<T> &v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

template<typename T>
inline T norm(const Vector3<T> &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template<typename T>
inline T norm(const Vector4<T> &v) {
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

template<typename T>
inline T norm(const Quaternion<T> &q) {
    return std::sqrt(q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w);
}

template<typename T>
inline T normalize(const T &v) {
    return v / norm(v);
}

/**
 * @note Dot product
 * <https://en.wikipedia.org/wiki/Dot_product>
 */
template<typename T>
inline T dot(const Vector2<T> &a, const Vector2<T> &b) {
    return a.x * b.x + a.y * b.y;
}

template<typename T>
inline T dot(const Vector3<T> &a, const Vector3<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
inline T dot(const Vector4<T> &a, const Vector4<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template<typename T>
constexpr T default_rel_tol = std::numeric_limits<T>::epsilon() * 128;

template<typename T>
constexpr T default_abs_tol = std::numeric_limits<T>::min();

/**
 * @brief
 *
 * @details rel_tol:
 *   Floating points have about 6-7 significant digits, so
 *   1.000001f can be represented while 1.0000001f is rounded to zero,
 *   thus we could use an epsilon of 0.000001f(1e-6) for comparing values
 *   close to 1. We multiply this epsilon by the biggest magnitude of a and b.
 *
 * @details implementation notes:
 *   * python
 *     * https://docs.python.org/3/library/math.html#math.isclose
 *   * unity
 *     * https://github.com/Unity-Technologies/UnityCsReference/blob/2023.1/Runtime/Export/Math/Mathf.cs#L279
 *   * numpy
 *     * https://numpy.org/doc/stable/reference/generated/numpy.isclose.html
 *   * articles
 *     * https://stackoverflow.com/questions/4915462/how-should-i-do-floating-point-comparison
 *
 * @param a
 * @param b
 * @param rel_tol the relative tolerance - it is the maximum allowed difference between a and b,
 *   relative to the larger absolute value of a or b. For example, to set a tolerance of 5%,
 *   pass rel_tol=0.05. The default tolerance is 1e-09, which assures that the two values
 *   are the same within about 9 decimal digits. rel_tol must be greater than zero.
 */
template<typename T,
         std::enable_if_t<std::is_floating_point<T>::value> * = nullptr>
bool approx_equal(T a, T b,
                  T rel_tol = default_rel_tol<T>,
                  T abs_tol = default_abs_tol<T>) {
    // For comparison with infinities.
    if (a == b) return true;
    // Clamp the value with max for handing infinity.
    const float norm = (std::min)((std::max)(std::abs(a), std::abs(b)), std::numeric_limits<T>::max());
    return std::abs(a - b) <= std::max(rel_tol * norm, abs_tol);
}

} // namespace math
} // namespace nodec

#endif