#ifndef NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__BASIC_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__BASIC_HPP_

#include <nodec_serialization/nodec/quaternion.hpp>
#include <nodec_serialization/nodec/vector3.hpp>

#include <cereal/types/polymorphic.hpp>

namespace nodec_scene {
namespace components {

class SerializableName : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableName()
        : BaseSerializableComponent(this) {
    }

    std::string name;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("name", name));
    }
};

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

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/xml.hpp>

CEREAL_REGISTER_TYPE(nodec_scene::components::SerializableName);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_scene::components::SerializableName);

CEREAL_REGISTER_TYPE(nodec_scene::components::SerializableTransform);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_scene::components::SerializableTransform);

#endif