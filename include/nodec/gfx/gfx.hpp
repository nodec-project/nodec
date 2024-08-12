#ifndef NODEC__GFX__GFX_HPP_
#define NODEC__GFX__GFX_HPP_

#include "../math/math.hpp"

namespace nodec {

/**
 * math utils for graphics
 */
namespace gfx {

/**
 * @brief Rotates the vector v by rotation q.
 *
 * v' = q * [v.x, v.y, v.z, 0] * conj(q)  (|q| = 1)
 */
inline Vector3f rotate(const Vector3f &v, const Quaternionf &q) {
    // implementation refs:
    //  * https://github.com/MonoGame/MonoGame/blob/develop/MonoGame.Framework/Vector3.cs#L1084
    float x = 2 * (q.y * v.z - q.z * v.y);
    float y = 2 * (q.z * v.x - q.x * v.z);
    float z = 2 * (q.x * v.y - q.y * v.x);

    return {
        v.x + x * q.w + (q.y * z - q.z * y),
        v.y + y * q.w + (q.z * x - q.x * z),
        v.z + z * q.w + (q.x * y - q.y * x)};
}

inline Matrix4x4f translation_matrix(const Vector3f &t) {
    return {
        1.0f, 0.0f, 0.0f, t.x,
        0.0f, 1.0f, 0.0f, t.y,
        0.0f, 0.0f, 1.0f, t.z,
        0.0f, 0.0f, 0.0f, 1.0f};
}

inline Quaternionf quaternion_from_rotation_matrix(const Matrix4x4f &matrix) {
    float sqrt;
    float half;
    float scale = matrix.m11 + matrix.m22 + matrix.m33;

    Quaternionf result;

    if (scale > 0.0f) {
        sqrt = std::sqrtf(scale + 1.0f);
        result.w = sqrt * 0.5f;
        sqrt = 0.5f / sqrt;

        result.x = (matrix.m32 - matrix.m23) * sqrt;
        result.y = (matrix.m13 - matrix.m31) * sqrt;
        result.z = (matrix.m21 - matrix.m12) * sqrt;
    } else if ((matrix.m11 >= matrix.m22) && (matrix.m11 >= matrix.m33)) {
        sqrt = std::sqrtf(1.0f + matrix.m11 - matrix.m22 - matrix.m33);
        half = 0.5f / sqrt;

        result.x = 0.5f * sqrt;
        result.y = (matrix.m21 + matrix.m12) * half;
        result.z = (matrix.m31 + matrix.m13) * half;
        result.w = (matrix.m32 - matrix.m23) * half;
    } else if (matrix.m22 > matrix.m33) {
        sqrt = std::sqrtf(1.0f + matrix.m22 - matrix.m11 - matrix.m33);
        half = 0.5f / sqrt;

        result.x = (matrix.m12 + matrix.m21) * half;
        result.y = 0.5f * sqrt;
        result.z = (matrix.m23 + matrix.m32) * half;
        result.w = (matrix.m13 - matrix.m31) * half;
    } else {
        sqrt = std::sqrtf(1.0f + matrix.m33 - matrix.m11 - matrix.m22);
        half = 0.5f / sqrt;

        result.x = (matrix.m13 + matrix.m31) * half;
        result.y = (matrix.m23 + matrix.m32) * half;
        result.z = 0.5f * sqrt;
        result.w = (matrix.m21 - matrix.m12) * half;
    }

    return result;
}

inline Matrix4x4f rotation_matrix_from_quaternion(const Quaternionf &r) {
    const float xx2 = 2.0f * r.x * r.x;
    const float yy2 = 2.0f * r.y * r.y;
    const float zz2 = 2.0f * r.z * r.z;
    const float xy2 = 2.0f * r.x * r.y;
    const float xz2 = 2.0f * r.x * r.z;
    const float yz2 = 2.0f * r.y * r.z;
    const float wx2 = 2.0f * r.w * r.x;
    const float wy2 = 2.0f * r.w * r.y;
    const float wz2 = 2.0f * r.w * r.z;

    return {
        (1.0f - yy2 - zz2), (xy2 - wz2), (xz2 + wy2), 0.f,
        (xy2 + wz2), (1.0f - xx2 - zz2), (yz2 - wx2), 0.f,
        (xz2 - wy2), (yz2 + wx2), (1.0f - xx2 - yy2), 0.f,
        0.0f, 0.0f, 0.0f, 1.0f};
}

inline Matrix4x4f trs(const Vector3f &t, const Quaternionf &r, const Vector3f &s) {
    const float xx2 = 2.0f * r.x * r.x;
    const float yy2 = 2.0f * r.y * r.y;
    const float zz2 = 2.0f * r.z * r.z;
    const float xy2 = 2.0f * r.x * r.y;
    const float xz2 = 2.0f * r.x * r.z;
    const float yz2 = 2.0f * r.y * r.z;
    const float wx2 = 2.0f * r.w * r.x;
    const float wy2 = 2.0f * r.w * r.y;
    const float wz2 = 2.0f * r.w * r.z;

    return {
        (1.0f - yy2 - zz2) * s.x, (xy2 - wz2) * s.y, (xz2 + wy2) * s.z, t.x,
        (xy2 + wz2) * s.x, (1.0f - xx2 - zz2) * s.y, (yz2 - wx2) * s.z, t.y,
        (xz2 - wy2) * s.x, (yz2 + wx2) * s.y, (1.0f - xx2 - yy2) * s.z, t.z,
        0.0f, 0.0f, 0.0f, 1.0f};
}

/**
 * @brief Decompose trs matrix to translation, rotation and scale elements.
 * Returns true if matrix can be decomposed; false otherwise.
 */
inline bool decompose_trs(const Matrix4x4f &trs, Vector3f &translation, Quaternionf &rotation, Vector3f &scale) {
    translation.x = trs.m14;
    translation.y = trs.m24;
    translation.z = trs.m34;

    float xs = (trs.m11 * trs.m21 * trs.m31 * trs.m41) < 0.f ? -1.f : 1.f;
    float ys = (trs.m12 * trs.m22 * trs.m32 * trs.m42) < 0.f ? -1.f : 1.f;
    float zs = (trs.m13 * trs.m23 * trs.m33 * trs.m43) < 0.f ? -1.f : 1.f;

    scale.x = xs * std::sqrtf(trs.m11 * trs.m11 + trs.m21 * trs.m21 + trs.m31 * trs.m31);
    scale.y = ys * std::sqrtf(trs.m12 * trs.m12 + trs.m22 * trs.m22 + trs.m32 * trs.m32);
    scale.z = zs * std::sqrtf(trs.m13 * trs.m13 + trs.m23 * trs.m23 + trs.m33 * trs.m33);

    if (scale.x == 0.f || scale.y == 0.f || scale.z == 0.f) {
        rotation = Quaternionf::identity;
        return false;
    }

    rotation = quaternion_from_rotation_matrix(
        {trs.m11 / scale.x, trs.m12 / scale.y, trs.m13 / scale.z, 0.f,
         trs.m21 / scale.x, trs.m22 / scale.y, trs.m23 / scale.z, 0.f,
         trs.m31 / scale.x, trs.m32 / scale.y, trs.m33 / scale.z, 0.f,
         0.f, 0.f, 0.f, 1.f});

    return true;
}

struct TRSComponents {
    Vector3f translation;
    Quaternionf rotation;
    Vector3f scale;
};

inline bool decompose_trs(const Matrix4x4f &trs, TRSComponents &components) {
    return decompose_trs(trs, components.translation, components.rotation, components.scale);
}

inline Quaternionf quaternion_from_angle_axis(const float &angle_deg, const Vector3f &axis) {
    auto theta = angle_deg * math::deg2rad<float> / 2.0f;
    auto s = std::sin(theta);
    auto a = math::normalize(axis);
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
        angles.y = std::copysign(90.f, sin_y);
    } else {
        angles.y = std::asin(sin_y);
    }

    // z-axis rotation
    float sin_z = 2 * (rotation.z * rotation.w - rotation.x * rotation.y);
    float cos_z = 2 * (rotation.x * rotation.x + rotation.w * rotation.w) - 1;
    angles.z = std::atan2(sin_z, cos_z);

    return angles * math::rad2deg<float>;
}

/**
 * Rx * Ry * Rz = R
 */
inline Quaternionf euler_angles_xyz(const Vector3f &euler) {
    float cx = std::cos(euler.x * 0.5f * math::deg2rad<float>);
    float sy = std::sin(euler.y * 0.5f * math::deg2rad<float>);
    float sz = std::sin(euler.z * 0.5f * math::deg2rad<float>);
    float sx = std::sin(euler.x * 0.5f * math::deg2rad<float>);
    float cy = std::cos(euler.y * 0.5f * math::deg2rad<float>);
    float cz = std::cos(euler.z * 0.5f * math::deg2rad<float>);

    return {
        cx * sy * sz + sx * cy * cz,
        cx * sy * cz - sx * cy * sz,
        cx * cy * sz + sx * sy * cz,
        cx * cy * cz - sx * sy * sz};
}

inline Quaternionf look_rotation(const Vector3f &forward, const Vector3f &upwards = Vector3f(0.f, 1.f, 0.f)) {
    // https://stackoverflow.com/questions/53143175/writing-a-lookat-function

    auto z = math::normalize(forward);
    auto x = math::cross(upwards, z);
    x = math::normalize(x);
    auto y = math::cross(z, x);
    y = math::normalize(y);

    Matrix4x4f matrix(
        x.x, y.x, z.x, 0.f,
        x.y, y.y, z.y, 0.f,
        x.z, y.z, z.z, 0.f,
        0.f, 0.f, 0.f, 1.f);
    return quaternion_from_rotation_matrix(matrix);
}

} // namespace gfx
} // namespace nodec

#endif