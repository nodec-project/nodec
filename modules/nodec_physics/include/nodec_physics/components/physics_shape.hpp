#ifndef NODEC_PHYSICS__COMPONENTS__PHYSICS_SHAPE_HPP_
#define NODEC_PHYSICS__COMPONENTS__PHYSICS_SHAPE_HPP_

#include <nodec/vector3.hpp>

namespace nodec_physics {
namespace components {

// NOTE: Component name considerations.
//  * PhysicsShape
//  * BoxCollider

struct PhysicsShape {
    enum class ShapeType {
        Box,
        Sphere
    };

    ShapeType shape_type{ShapeType::Box};

    nodec::Vector3f size{1.0f, 1.0f, 1.0f};
};

} // namespace components
} // namespace nodec_physics

#endif