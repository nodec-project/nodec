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
 * @brief Fast absolute value implementation
 *
 * A simple, efficient replacement for std::abs that avoids function call overhead.
 * This implementation uses a basic conditional expression which most compilers
 * can optimize effectively, particularly for branchless execution.
 *
 * @param x The value to get the absolute value of
 * @return The absolute value of x
 */
template<typename T>
constexpr inline T abs(T x) {
    return x < 0 ? -x : x;
}

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
inline constexpr T normalize(const T &v) {
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
inline constexpr T dot(const Vector3<T> &a, const Vector3<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<typename T>
inline T dot(const Vector4<T> &a, const Vector4<T> &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

/**
 * @brief Calculate cross product.
 *
 * @note This equation is same on left-handed and right-handed coordinate system.
 */
template<typename T>
inline Vector3<T> cross(const Vector3<T> &a, const Vector3<T> &b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

template<typename T>
constexpr T default_rel_tol = std::numeric_limits<T>::epsilon() * 128;

template<typename T>
constexpr T default_abs_tol = (std::numeric_limits<T>::min)();

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
    const float norm = (std::min)((std::max)(abs(a), abs(b)), (std::numeric_limits<T>::max)());
    return abs(a - b) <= (std::max)(rel_tol * norm, abs_tol);
}

template<typename T,
         std::enable_if_t<std::is_floating_point<T>::value> * = nullptr>
bool approx_equal(const Vector3<T> &a, const Vector3<T> &b,
                  T rel_tol = default_rel_tol<T>,
                  T abs_tol = default_abs_tol<T>) {
    return approx_equal(a.x, b.x, rel_tol, abs_tol)
           && approx_equal(a.y, b.y, rel_tol, abs_tol)
           && approx_equal(a.z, b.z, rel_tol, abs_tol);
}

template<typename T,
         std::enable_if_t<std::is_floating_point<T>::value> * = nullptr>
bool approx_equal(const Quaternion<T> &a, const Quaternion<T> &b,
                  T rel_tol = default_rel_tol<T>,
                  T abs_tol = default_abs_tol<T>) {
    return approx_equal(a.x, b.x, rel_tol, abs_tol)
           && approx_equal(a.y, b.y, rel_tol, abs_tol)
           && approx_equal(a.z, b.z, rel_tol, abs_tol)
           && approx_equal(a.w, b.w, rel_tol, abs_tol);
}

template<typename T,
         std::enable_if_t<std::is_floating_point<T>::value> * = nullptr>
bool approx_equal_rotation(const Quaternion<T> &a, const Quaternion<T> &b,
                           T rel_tol = default_rel_tol<T>,
                           T abs_tol = default_abs_tol<T>) {
    // https://gamedev.stackexchange.com/questions/75072/how-can-i-compare-two-quaternions-for-logical-equality
    return approx_equal(a, b, rel_tol, abs_tol) || approx_equal(a, -b, rel_tol, abs_tol);
}

template<typename T,
         std::enable_if_t<std::is_floating_point<T>::value> * = nullptr>
bool approx_equal(const Matrix4x4<T> &a, const Matrix4x4<T> &b,
                  T rel_tol = default_rel_tol<T>,
                  T abs_tol = default_abs_tol<T>) {
    return approx_equal(a.m[0], b.m[0], rel_tol, abs_tol)
           && approx_equal(a.m[1], b.m[1], rel_tol, abs_tol)
           && approx_equal(a.m[2], b.m[2], rel_tol, abs_tol)
           && approx_equal(a.m[3], b.m[3], rel_tol, abs_tol)
           && approx_equal(a.m[4], b.m[4], rel_tol, abs_tol)
           && approx_equal(a.m[5], b.m[5], rel_tol, abs_tol)
           && approx_equal(a.m[6], b.m[6], rel_tol, abs_tol)
           && approx_equal(a.m[7], b.m[7], rel_tol, abs_tol)
           && approx_equal(a.m[8], b.m[8], rel_tol, abs_tol)
           && approx_equal(a.m[9], b.m[9], rel_tol, abs_tol)
           && approx_equal(a.m[10], b.m[10], rel_tol, abs_tol)
           && approx_equal(a.m[11], b.m[11], rel_tol, abs_tol)
           && approx_equal(a.m[12], b.m[12], rel_tol, abs_tol)
           && approx_equal(a.m[13], b.m[13], rel_tol, abs_tol)
           && approx_equal(a.m[14], b.m[14], rel_tol, abs_tol)
           && approx_equal(a.m[15], b.m[15], rel_tol, abs_tol);
}

template<typename T>
Quaternion<T> inv(const Quaternion<T> &q) {
    const float num2 = (((q.x * q.x) + (q.y * q.y)) + (q.z * q.z)) + (q.w * q.w);
    const float num = 1.f / num2;

    return {-q.x * num, -q.y * num, -q.z * num, q.w * num};
}

template<typename T>
Matrix4x4<T> inv(const Matrix4x4<T> &mat, T *determinant = nullptr) {
    // Optimized 4x4 matrix inverse using 2x2 sub-determinants
    // This algorithm pre-computes 12 2x2 determinants and reuses them,
    // reducing multiplications from ~144 to ~100.
    //
    // Matrix4x4 is column-major: m[col*4 + row]
    // m[0-3] = column 0 (m11,m21,m31,m41), m[4-7] = column 1, etc.
    auto &m = mat.m;

    // Pre-compute 2x2 determinants from rows 0-1
    const T b00 = m[0] * m[5] - m[4] * m[1];   // m11*m22 - m12*m21
    const T b01 = m[0] * m[9] - m[8] * m[1];   // m11*m23 - m13*m21
    const T b02 = m[0] * m[13] - m[12] * m[1]; // m11*m24 - m14*m21
    const T b03 = m[4] * m[9] - m[8] * m[5];   // m12*m23 - m13*m22
    const T b04 = m[4] * m[13] - m[12] * m[5]; // m12*m24 - m14*m22
    const T b05 = m[8] * m[13] - m[12] * m[9]; // m13*m24 - m14*m23

    // Pre-compute 2x2 determinants from rows 2-3
    const T b06 = m[2] * m[7] - m[6] * m[3];     // m31*m42 - m32*m41
    const T b07 = m[2] * m[11] - m[10] * m[3];   // m31*m43 - m33*m41
    const T b08 = m[2] * m[15] - m[14] * m[3];   // m31*m44 - m34*m41
    const T b09 = m[6] * m[11] - m[10] * m[7];   // m32*m43 - m33*m42
    const T b10 = m[6] * m[15] - m[14] * m[7];   // m32*m44 - m34*m42
    const T b11 = m[10] * m[15] - m[14] * m[11]; // m33*m44 - m34*m43

    // Compute determinant
    const T det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

    if (determinant != nullptr) {
        *determinant = det;
    }

    const T inv_det = T(1) / det;

    // Compute inverse matrix elements (column-major output)
    return {
        (m[5] * b11 - m[9] * b10 + m[13] * b09) * inv_det,   // inv_m11
        (m[8] * b10 - m[4] * b11 - m[12] * b09) * inv_det,   // inv_m12
        (m[7] * b05 - m[11] * b04 + m[15] * b03) * inv_det,  // inv_m13
        (m[10] * b04 - m[6] * b05 - m[14] * b03) * inv_det,  // inv_m14
        (m[9] * b08 - m[1] * b11 - m[13] * b07) * inv_det,   // inv_m21
        (m[0] * b11 - m[8] * b08 + m[12] * b07) * inv_det,   // inv_m22
        (m[11] * b02 - m[3] * b05 - m[15] * b01) * inv_det,  // inv_m23
        (m[2] * b05 - m[10] * b02 + m[14] * b01) * inv_det,  // inv_m24
        (m[1] * b10 - m[5] * b08 + m[13] * b06) * inv_det,   // inv_m31
        (m[4] * b08 - m[0] * b10 - m[12] * b06) * inv_det,   // inv_m32
        (m[3] * b04 - m[7] * b02 + m[15] * b00) * inv_det,   // inv_m33
        (m[6] * b02 - m[2] * b04 - m[14] * b00) * inv_det,   // inv_m34
        (m[5] * b07 - m[1] * b09 - m[9] * b06) * inv_det,    // inv_m41
        (m[0] * b09 - m[4] * b07 + m[8] * b06) * inv_det,    // inv_m42
        (m[7] * b01 - m[3] * b03 - m[11] * b00) * inv_det,   // inv_m43
        (m[2] * b03 - m[6] * b01 + m[10] * b00) * inv_det};  // inv_m44
}

} // namespace math
} // namespace nodec

#endif