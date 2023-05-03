#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POINT_LIGHT_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__POINT_LIGHT_HPP_

#include <nodec_rendering/components/point_light.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>
#include <nodec_serialization/nodec/vector4.hpp>

namespace nodec_rendering {
namespace components {

class SerializablePointLight : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializablePointLight()
        : BaseSerializableComponent(this) {
    }

    SerializablePointLight(const PointLight &other)
        : BaseSerializableComponent(this),
          color(other.color),
          intensity(other.intensity),
          range(other.range) {}

    operator PointLight() const noexcept {
        PointLight value;
        value.color = color;
        value.intensity = intensity;
        value.range = range;
        return value;
    }

    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};
    float intensity{1.0f};
    float range{10.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("color", color));
        archive(cereal::make_nvp("intensity", intensity));
        archive(cereal::make_nvp("range", range));
    }
};

} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializablePointLight)

#endif