#ifndef NODEC__GFX__FRUSTUM_HPP_
#define NODEC__GFX__FRUSTUM_HPP_

#include "../math/math.hpp"
#include "../vector3.hpp"
#include "bouding_box.hpp"

namespace nodec {
namespace gfx {

struct Plane {
    //! Normal vector (Unit vector) of the plane.
    Vector3f normal{0.f, 1.f, 0.f};

    //! Distance from original to the nearest point of the plane.
    float distance{0.f};

    constexpr Plane(const Vector3f &p, const Vector3f &n)
        : normal(math::normalize(n)), distance(math::dot(p, normal)) {}

    constexpr Plane()
        : normal{0.f, 1.f, 0.f}, distance{0.f} {}
};

struct Frustum {
    union {
        struct {
            Plane near_plane;
            Plane far_plane;
            Plane left_plane;
            Plane right_plane;
            Plane top_plane;
            Plane bottom_plane;
        };
        Plane planes[6];
    };
    constexpr Frustum()
        : near_plane{}, far_plane{}, left_plane{}, right_plane{}, top_plane{}, bottom_plane{} {}
};

/**
 * @brief Set the frustum from camera lh object
 *
 * @param position
 * @param forward
 * @param up
 * @param right
 * @param aspect w/h
 * @param fov
 * @param z_near
 * @param z_far
 * @param frustum
 */
inline void set_frustum_from_camera_lh(const Vector3f &position,
                                       const Vector3f forward, const Vector3f &up, const Vector3f &right,
                                       float aspect, float fov, float z_near, float z_far, Frustum &frustum) {
    const float half_v_side = z_far * std::tanhf(fov * .5f);
    const float half_h_side = half_v_side * aspect;
    Vector3f front_mult_far = z_far * forward;

    frustum.near_plane = Plane{position + z_near * forward, forward};
    frustum.far_plane = Plane{position + front_mult_far, -forward};
    frustum.right_plane = Plane{
        position, math::cross(front_mult_far + right * half_h_side, up)};
    frustum.left_plane = Plane{
        position, math::cross(front_mult_far - right * half_h_side, -up)};
    frustum.top_plane = Plane{
        position, math::cross(front_mult_far + up * half_v_side, -right)};
    frustum.bottom_plane = Plane{
        position, math::cross(front_mult_far - up * half_v_side, right)};
}

inline float distance(const Plane &plane, const Vector3f &point) {
    return math::dot(plane.normal, point) - plane.distance;
}

inline bool intersects(const Plane &plane, const BoundingBox &bounds) {
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    const float r = bounds.extents.x * std::abs(plane.normal.x) + bounds.extents.y * std::abs(plane.normal.y) + bounds.extents.z * std::abs(plane.normal.z);
    return -r <= distance(plane, bounds.center);
}

inline bool intersects(const Frustum &frustum, const BoundingBox &bounds, const Matrix4x4f &bounds_local_to_world) {
    const Vector4f global_center = bounds_local_to_world * Vector4f(bounds.center, 1.0f);

    const Vector3f right = Vector3f(bounds_local_to_world.m[0], bounds_local_to_world.m[1], bounds_local_to_world.m[2]) * bounds.extents.x;
    const Vector3f up = Vector3f(bounds_local_to_world.m[4], bounds_local_to_world.m[5], bounds_local_to_world.m[6]) * bounds.extents.y;
    const Vector3f forward = Vector3f(bounds_local_to_world.m[8], bounds_local_to_world.m[9], bounds_local_to_world.m[10]) * bounds.extents.z;

    const float new_ii = std::abs(math::dot(Vector3f(1.f, 0.f, 0.f), right))
                         + std::abs(math::dot(Vector3f(1.f, 0.f, 0.f), up))
                         + std::abs(math::dot(Vector3f(1.f, 0.f, 0.f), forward));

    const float new_ij = std::abs(math::dot(Vector3f(0.f, 1.f, 0.f), right))
                         + std::abs(math::dot(Vector3f(0.f, 1.f, 0.f), up))
                         + std::abs(math::dot(Vector3f(0.f, 1.f, 0.f), forward));

    const float new_ik = std::abs(math::dot(Vector3f(0.f, 0.f, 1.f), right))
                         + std::abs(math::dot(Vector3f(0.f, 0.f, 1.f), up))
                         + std::abs(math::dot(Vector3f(0.f, 0.f, 1.f), forward));

    const BoundingBox global_bounds(Vector3f(global_center.x, global_center.y, global_center.z), Vector3f(new_ii, new_ij, new_ik));

    return (intersects(frustum.left_plane, global_bounds)
            && intersects(frustum.right_plane, global_bounds)
            && intersects(frustum.top_plane, global_bounds)
            && intersects(frustum.bottom_plane, global_bounds)
            && intersects(frustum.near_plane, global_bounds)
            && intersects(frustum.far_plane, global_bounds));
}

} // namespace gfx
} // namespace nodec

#endif