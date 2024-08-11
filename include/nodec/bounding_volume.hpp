#ifndef NODEC__BOUNDING_VOLUME_HPP_
#define NODEC__BOUNDING_VOLUME_HPP_

#include <nodec/vector3.hpp>

namespace nodec {

struct BoundingBox {
    static constexpr BoundingBox from_minmax(const Vector3f& min, const Vector3f& max) {
        BoundingBox result;
        result.set_minmax(min, max);
        return result;
    }

    Vector3f center;
    Vector3f extents;

    constexpr Vector3f (min)() const {
        return center - extents;
    }

    constexpr Vector3f (max)() const {
        return center + extents;
    }

    constexpr void set_minmax(const Vector3f& min, const Vector3f& max) {
        center = (min + max) / 2.0f;
        extents = (max - min) / 2.0f;
    }
};

inline constexpr bool operator==(const BoundingBox& lhs, const BoundingBox& rhs) {
    return lhs.center == rhs.center && lhs.extents == rhs.extents;
}

inline constexpr bool operator!=(const BoundingBox& lhs, const BoundingBox& rhs) {
    return !(lhs == rhs);
}

} // namespace nodec

#endif