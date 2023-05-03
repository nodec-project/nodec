#ifndef NODEC_SERIALIZATION__NODEC_PHYSICS__COMPONENTS__RIGID_BODY_HPP_
#define NODEC_SERIALIZATION__NODEC_PHYSICS__COMPONENTS__RIGID_BODY_HPP_

#include <nodec_physics/components/rigid_body.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_physics {
namespace components {

class SerializableRigidBody : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableRigidBody()
        : BaseSerializableComponent(this) {}

    SerializableRigidBody(const RigidBody &other)
        : BaseSerializableComponent(this),
          mass(other.mass) {}

    operator RigidBody() const noexcept {
        RigidBody value;
        value.mass = mass;
        return value;
    }

    float mass{0.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("mass", mass));
    }
};
} // namespace components
} // namespace nodec_physics

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_physics::components::SerializableRigidBody)

#endif