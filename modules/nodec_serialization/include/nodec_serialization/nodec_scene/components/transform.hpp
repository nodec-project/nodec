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

    SerializableTransform(const Transform &other)
        : BaseSerializableComponent(this),
          local_position(other.position),
          local_rotation(other.rotation),
          local_scale(other.scale) {}

    operator Transform() const noexcept {
        Transform value;
        value.position = local_position;
        value.rotation = local_rotation;
        value.scale = local_scale;
        value.dirty = true;
        return value;
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

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene::components::SerializableTransform)

#endif