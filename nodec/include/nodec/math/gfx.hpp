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
    auto a = angle_deg * deg2rad<float> / 2.0f;
    auto s = std::sin(a);
    quaternion.x = axis.x * s;
    quaternion.y = axis.y * s;
    quaternion.z = axis.z * s;
    quaternion.w = std::cos(a);
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