#ifndef NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__NAME_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__NAME_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

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

} // namespace components
} // namespace nodec_scene

CEREAL_REGISTER_TYPE(nodec_scene::components::SerializableName)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_scene::components::SerializableName)

#endif