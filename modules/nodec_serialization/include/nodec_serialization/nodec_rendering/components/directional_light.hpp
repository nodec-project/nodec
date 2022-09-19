#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__DIRECTIONAL_LIGHT_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__DIRECTIONAL_LIGHT_HPP_

#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializableDirectionalLight : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableDirectionalLight()
        : BaseSerializableComponent(this) {
    }

    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("color", color));
        archive(cereal::make_nvp("intensity", intensity));
    }
private:
    void rtti() {}
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableDirectionalLight);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableDirectionalLight);

#endif