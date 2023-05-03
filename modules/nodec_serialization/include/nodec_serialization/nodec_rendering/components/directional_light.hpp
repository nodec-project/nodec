#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__DIRECTIONAL_LIGHT_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__DIRECTIONAL_LIGHT_HPP_

#include <nodec_rendering/components/directional_light.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

namespace nodec_rendering {
namespace components {

class SerializableDirectionalLight : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableDirectionalLight()
        : BaseSerializableComponent(this) {
    }

    SerializableDirectionalLight(const DirectionalLight &other)
        : BaseSerializableComponent(this),
          color(other.color),
          intensity(other.intensity) {}

    operator DirectionalLight() const noexcept {
        DirectionalLight value;
        value.color = color;
        value.intensity = intensity;
        return value;
    }

    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("color", color));
        archive(cereal::make_nvp("intensity", intensity));
    }
};

} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializableDirectionalLight)

#endif