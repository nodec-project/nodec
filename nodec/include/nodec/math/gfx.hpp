#ifndef NODEC__MATH__GFX_HPP_
#define NODEC__MATH__GFX_HPP_

#include <nodec/math/math.hpp>

namespace nodec {
namespace math {

/**
 * math utils for graphics
 */
namespace gfx {

inline Vector3f transform(const Vector3f &vec, const Quaternionf &q) {
    Quaternionf qv(vec.x, vec.y, vec.z, 0.0f);
    auto vb = q * Quaternionf(vec.x, vec.y, vec.z, 0.0f) * conj(q);
    return Vector3f(vb.x, vb.y, vb.z);
}

inline Matrix4x4f trs(const Vector3f &t, const Quaternionf &r, const Vector3f &s) {
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
        0.0f, 0.0f, 0.0f, 1.0f};
}

inline Quaternionf angle_axis(const float &angle_deg, const Vector3f &axis) {
    auto theta = angle_deg * deg2rad<float> / 2.0f;
    auto s = std::sin(theta);
    auto a = normalize(axis);
    return {a.x * s, a.y * s, a.z * s, std::cos(theta)};
}

// * <https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles>
//  the equation of above page is zyx conversion.

inline Vector3f euler_angles_xyz(const Quaternionf &rotation) {
    Vector3f angles;

    // x-axis rotation
    float sin_x = 2 * (rotation.x * rotation.w - rotation.y * rotation.z);
    float cos_x = 2 * (rotation.z * rotation.z + rotation.w * rotation.w) - 1;
    angles.x = std::atan2(sin_x, cos_x);

    // y-axis rotation
    float sin_y = 2 * (rotation.x * rotation.z + rotation.y * rotation.w);
    if (std::abs(sin_y) >= 1) {
        // use 90 degrees if out of range
        angles.y = std::copysign(90, sin_y);
    } else {
        angles.y = std::asin(sin_y);
    }

    // z-axis rotation
    float sin_z = 2 * (rotation.z * rotation.w - rotation.x * rotation.y);
    float cos_z = 2 * (rotation.x * rotation.x + rotation.w * rotation.w) - 1;
    angles.z = std::atan2(sin_z, cos_z);

    return angles * rad2deg<float>;
}

/**
 * Rx * Ry * Rz = R
 */
inline void set_eulaer_angles_xyz(Quaternionf &rotation, const Vector3f &euler) {
    float cx = std::cos(euler.x * 0.5f * deg2rad<float>);
    float sy = std::sin(euler.y * 0.5f * deg2rad<float>);
    float sz = std::sin(euler.z * 0.5f * deg2rad<float>);
    float sx = std::sin(euler.x * 0.5f * deg2rad<float>);
    float cy = std::cos(euler.y * 0.5f * deg2rad<float>);
    float cz = std::cos(euler.z * 0.5f * deg2rad<float>);

    rotation.x = cx * sy * sz + sx * cy * cz;
    rotation.y = cx * sy * cz - sx * cy * sz;
    rotation.z = cx * cy * sz + sx * sy * cz;
    rotation.w = cx * cy * cz - sx * sy * sz;
}

} // namespace gfx
} // namespace math
} // namespace nodec

#endif