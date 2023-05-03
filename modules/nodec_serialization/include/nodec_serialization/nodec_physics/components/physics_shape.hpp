#ifndef NODEC_SERIALIZATION__NODEC_PHYSICS__COMPONENTS__PHYSICS_SHAPE_HPP_
#define NODEC_SERIALIZATION__NODEC_PHYSICS__COMPONENTS__PHYSICS_SHAPE_HPP_

#include <nodec_physics/components/physics_shape.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_physics {
namespace components {

class SerializablePhysicsShape : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializablePhysicsShape()
        : BaseSerializableComponent(this) {}

    SerializablePhysicsShape(const PhysicsShape &other)
        : BaseSerializableComponent(this),
          shape_type(other.shape_type),
          size(other.size),
          radius(other.radius) {}

    operator PhysicsShape() const noexcept {
        PhysicsShape value;
        value.shape_type = shape_type;
        value.size = size;
        value.radius = radius;
        return value;
    }

    PhysicsShape::ShapeType shape_type{PhysicsShape::ShapeType::Box};

    nodec::Vector3f size{1.0f, 1.0f, 1.0f};

    float radius{0.5f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("shape_type", shape_type));
        archive(cereal::make_nvp("size", size));
        archive(cereal::make_nvp("radius", radius));
    }
};

} // namespace components
} // namespace nodec_physics

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_physics::components::SerializablePhysicsShape)

#endif