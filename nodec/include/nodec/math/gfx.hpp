#ifndef NODEC__MATH__GFX_HPP_
#define NODEC__MATH__GFX_HPP_

#include <nodec/math/math.hpp>

namespace nodec {
namespace math {

/**
* math utils for graphics
*/
namespace gfx {

inline void set_angle_axis(Quaternionf& quaternion, float angle_deg, const Vector3f& axis) {
    auto theta = angle_deg * deg2rad<float> / 2.0f;
    auto s = std::sin(theta);
    auto a = normalize(axis);
    quaternion.x = a.x * s;
    quaternion.y = a.y * s;
    quaternion.z = a.z * s;
    quaternion.w = std::cos(theta);
}

inline Vector3f transform(const Vector3f& vec, const Quaternionf& q) {
    Quaternionf qv(vec.x, vec.y, vec.z, 0.0f);
    auto vb = q * Quaternionf(vec.x, vec.y, vec.z, 0.0f) * conj(q);
    return Vector3f(vb.x, vb.y, vb.z);
}


inline Matrix4x4f trs(const Vector3f& t, const Quaternionf& r, const Vector3f& s) {
    float xx2 = 2.0f * r.x * r.x;
    float yy2 = 2.0f * r.y * r.y;
    float zz2 = 2.0f * r.z * r.z;
    float xy2 = 2.0f * r.x * r.y;
    float xz2 = 2.0f * r.x * r.z;
    float yz2 = 2.0f * r.y * r.z;
    float wx2 = 2.0f * r.w * r.x;
    float wy2 = 2.0f * r.w * r.y;
    float wz2 = 2.0f * r.w * r.z;

    return {
        (1.0f - yy2 - zz2) * s.x, (xy2 - wz2) * s.y, (xz2 + wy2) * s.z, t.x,
        (xy2 + wz2) * s.x, (1.0f - xx2 - zz2) * s.y, (yz2 - wx2) * s.z, t.y,
        (xz2 - wy2) * s.x, (yz2 + wx2) * s.y, (1.0f - xx2 - yy2) * s.z, t.z,
        0.0f, 0.0f, 0.0f, 1.0f
    };
}

//
//inline Matrix4x4f perspective(float fov, float aspect, float znear, float zfar) {
//    float f = 1.0f / std::tanf(fov / 2.0f * deg2rad<float>);
//    return{
//        f / aspect, 0.0f, 0.0f, 0.0f,
//        0.0f, f, 0.0f, 0.0f,
//        0.0f, 0.0f, (zfar + znear) / (znear - zfar), 2.0f * (zfar * znear) / (znear - zfar),
//        0.0f, 0.0f, -1.0f, 0.0f
//    };
//}
//
//inline Matrix4x4f ortho(float left, float right, float bottom, float top, float znear, float zfar) {
//    return{
//        2.0f / (right - left), 0.0f, 0.0f, -1.0f * (right + left) / (right - left),
//        0.0f, 2.0f / (top - bottom), 0.0f, -1.0f * (top + bottom) / (top - bottom),
//        0.0f, 0.0f, -2.0f / (zfar - znear), -1.0f * (zfar + znear) / (zfar - znear),
//        0.0f, 0.0f, 0.0f, 1.0f
//    };
//}

} // namespace gfx
} // namespace math
} // namespace nodec

#endif