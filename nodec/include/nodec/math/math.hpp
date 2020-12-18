#ifndef NODEC__MATH__MATH_HPP_
#define NODEC__MATH__MATH_HPP_

#include <nodec/vector2.hpp>
#include <nodec/vector3.hpp>
#include <nodec/vector4.hpp>
#include <nodec/quaternion.hpp>

#include <cmath>

namespace nodec
{
namespace math
{

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
inline Quaternion<T> conj(const Quaternion<T>& q)
{
    return Quaternion<T>(-q.x, -q.y, -q.z, q.w);
}


/**
* @note
*   <https://en.wikipedia.org/wiki/Norm_(mathematics)>
*/
template<typename T>
inline T norm(const Vector2<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

template<typename T>
inline T norm(const Vector3<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

template<typename T>
inline T norm(const Vector4<T>& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}

}
}

#endif