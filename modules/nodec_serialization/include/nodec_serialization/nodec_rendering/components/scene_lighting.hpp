#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__SCENE_LIGHTING_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__SCENE_LIGHTING_HPP_

#include <nodec_rendering/components/scene_lighting.hpp>

#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableSceneLighting : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableSceneLighting()
        : BaseSerializableComponent(this) {}

    nodec::Vector4f ambient_color;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("ambient_color", ambient_color));
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableSceneLighting)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableSceneLighting)

#endif