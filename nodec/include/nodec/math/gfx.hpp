#ifndef NODEC__MATH__GFX_HPP_
#define NODEC__MATH__GFX_HPP_

#include <nodec/math/math.hpp>

namespace nodec
{
namespace math
{

/**
* math utils for graphics
*/
namespace gfx
{

inline void set_angle_axis(Quaternionf& quaternion, float angle_deg, const Vector3f& axis)
{
    auto theta = angle_deg * deg2rad<float> / 2.0f;
    auto s = std::sin(theta);
    auto a = normalize(axis);
    quaternion.x = a.x * s;
    quaternion.y = a.y * s;
    quaternion.z = a.z * s;
    quaternion.w = std::cos(theta);
}

inline Vector3f transform(const Vector3f& vec, const Quaternionf& q)
{
    Quaternionf qv(vec.x, vec.y, vec.z, 0.0f);
    auto vb = q * Quaternionf(vec.x, vec.y, vec.z, 0.0f) * conj(q);
    return Vector3f(vb.x, vb.y, vb.z);
}

} // namespace gfx
} // namespace math
} // namespace nodec

#endif