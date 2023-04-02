#ifndef NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__NAME_HPP_
#define NODEC_SERIALIZATION__NODEC_SCENE__COMPONENTS__NAME_HPP_

#include <nodec_scene/components/name.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_scene {
namespace components {

class SerializableName : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableName()
        : BaseSerializableComponent(this) {
    }

    SerializableName(const Name &other)
        : BaseSerializableComponent(this), name(other.name) {}

    operator Name() const noexcept {
        Name value;
        value.name = name;
        return value;
    }

    std::string name;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("name", name));
    }
};

} // namespace components
} // namespace nodec_scene

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene::components::SerializableName)

#endif