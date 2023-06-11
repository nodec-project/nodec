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
        : BaseSerializableComponent(this), value(other.value) {}

    operator Name() const noexcept {
        Name name;
        name.value = value;
        return name;
    }

    std::string value;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("value", value));
    }
};

} // namespace components
} // namespace nodec_scene

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_scene::components::SerializableName)

#endif