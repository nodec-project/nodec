#ifndef NODEC__GFX__BOUNDING_BOX_HPP_
#define NODEC__GFX__BOUNDING_BOX_HPP_

#include "../vector3.hpp"

namespace nodec {
namespace gfx {

struct BoundingBox {
    static constexpr BoundingBox from_minmax(const Vector3f &min, const Vector3f &max) {
        BoundingBox result;
        result.set_minmax(min, max);
        return result;
    }

    Vector3f center;
    Vector3f extents;

    constexpr BoundingBox()
        : center{0.f, 0.f, 0.f}, extents{0.f, 0.f, 0.f} {}

    constexpr BoundingBox(const Vector3f &center, const Vector3f &extents)
        : center(center), extents(extents) {}

    constexpr Vector3f(min)() const {
        return center - extents;
    }

    constexpr Vector3f(max)() const {
        return center + extents;
    }

    constexpr void set_minmax(const Vector3f &min, const Vector3f &max) {
        center = (min + max) / 2.0f;
        extents = (max - min) / 2.0f;
    }
};

inline constexpr bool operator==(const BoundingBox &lhs, const BoundingBox &rhs) {
    return lhs.center == rhs.center && lhs.extents == rhs.extents;
}

inline constexpr bool operator!=(const BoundingBox &lhs, const BoundingBox &rhs) {
    return !(lhs == rhs);
}

} // namespace gfx
} // namespace nodec
#endif