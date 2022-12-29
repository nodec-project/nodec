#ifndef NODEC_SERIALIZATION__NODEC_PHYSICS__COMPONENTS__RIGID_BODY_HPP_
#define NODEC_SERIALIZATION__NODEC_PHYSICS__COMPONENTS__RIGID_BODY_HPP_

#include <nodec_scene_serialization/base_serializable_component.hpp>
#include <nodec_physics/components/rigid_body.hpp>

namespace nodec_physics {
namespace components {

class SerializableRigidBody : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableRigidBody()
        : BaseSerializableComponent(this) {}

    float mass{0.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("mass", mass));
    }
};
} // namespace components
} // namespace nodec_physics

CEREAL_REGISTER_TYPE(nodec_physics::components::SerializableRigidBody)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_physics::components::SerializableRigidBody)

#endif