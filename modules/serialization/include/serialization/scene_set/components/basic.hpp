#ifndef SERIALIZATION__SCENE_SET__COMPONENTS__BASIC_HPP_
#define SERIALIZATION__SCENE_SET__COMPONENTS__BASIC_HPP_

#include <serialization/nodec/vector3.hpp>
#include <serialization/nodec/quaternion.hpp>

#include <cereal/types/polymorphic.hpp>


namespace scene_set {
namespace components {

class SerializableName : public scene_serialization::BaseSerializableComponent {
public:
    SerializableName()
        : BaseSerializableComponent(this) {

    }

    std::string name;

    template<class Archive>
    void serialize(Archive& archive) {

        archive(cereal::make_nvp("name", name));

    }
};



class SerializableTransform : public scene_serialization::BaseSerializableComponent {
public:
    SerializableTransform()
        : BaseSerializableComponent(this) {

    }

    nodec::Vector3f local_position;
    nodec::Vector3f local_scale;
    nodec::Quaternionf local_rotation;


    template<class Archive>
    void serialize(Archive& archive) {

        archive(cereal::make_nvp("local_position", local_position));
        archive(cereal::make_nvp("local_scale", local_scale));
        archive(cereal::make_nvp("local_rotation", local_rotation));

    }
};



}
}

#include <cereal/archives/json.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/xml.hpp>


CEREAL_REGISTER_TYPE(scene_set::components::SerializableName);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, scene_set::components::SerializableName);

CEREAL_REGISTER_TYPE(scene_set::components::SerializableTransform);
CEREAL_REGISTER_POLYMORPHIC_RELATION(scene_serialization::BaseSerializableComponent, scene_set::components::SerializableTransform);


#endif