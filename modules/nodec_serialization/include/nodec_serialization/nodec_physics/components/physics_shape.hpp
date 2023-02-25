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

CEREAL_REGISTER_TYPE(nodec_physics::components::SerializablePhysicsShape)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_physics::components::SerializablePhysicsShape)

#endif