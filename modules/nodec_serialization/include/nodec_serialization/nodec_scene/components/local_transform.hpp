#ifndef NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__LOCAL_TRANSFORM_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__LOCAL_TRANSFORM_HPP_

#include <nodec_scene/components/local_transform.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>
#include <nodec_serialization/nodec/quaternion.hpp>
#include <nodec_serialization/nodec/vector3.hpp>

namespace nodec_scene {
namespace components {

class SerializableLocalTransform : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableLocalTransform()
        : BaseSerializableComponent(this) {
    }

    SerializableLocalTransform(const LocalTransform &other)
        : BaseSerializableComponent(this),
          position(other.position),
          rotation(other.rotation),
          scale(other.scale) {}

    operator LocalTransform() const noexcept {
        LocalTransform value;
        value.position = position;
        value.rotation = rotation;
        value.scale = scale;
        value.dirty = true;
        return value;
    }

    nodec::Vector3f position;
    nodec::Quaternionf rotation;
    nodec::Vector3f scale;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("position", position));
        archive(cereal::make_nvp("rotation", rotation));
        archive(cereal::make_nvp("scale", scale));
    }
};
} // namespace components
} // namespace nodec_scene

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene::components::SerializableLocalTransform)

#endif