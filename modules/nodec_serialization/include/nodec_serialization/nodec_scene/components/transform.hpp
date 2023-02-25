#ifndef NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__TRANSFORM_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__TRANSFORM_HPP_

#include <nodec_scene/components/transform.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>
#include <nodec_serialization/nodec/quaternion.hpp>
#include <nodec_serialization/nodec/vector3.hpp>

namespace nodec_scene {
namespace components {

class SerializableTransform : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableTransform()
        : BaseSerializableComponent(this) {
    }

    nodec::Vector3f local_position;
    nodec::Vector3f local_scale;
    nodec::Quaternionf local_rotation;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("local_position", local_position));
        archive(cereal::make_nvp("local_scale", local_scale));
        archive(cereal::make_nvp("local_rotation", local_rotation));
    }
};
} // namespace components
} // namespace nodec_scene

CEREAL_REGISTER_TYPE(nodec_scene::components::SerializableTransform)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_scene::components::SerializableTransform)

#endif