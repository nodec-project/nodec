#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__SCENE_LIGHTING_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__SCENE_LIGHTING_HPP_

#include <nodec_rendering/components/scene_lighting.hpp>
#include <nodec_rendering/resources/material.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableSceneLighting : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableSceneLighting()
        : BaseSerializableComponent(this) {}

    nodec::Vector4f ambient_color;
    std::shared_ptr<resources::Material> skybox;

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        archive(cereal::make_nvp("ambient_color", ambient_color));
        archive(cereal::make_nvp("skybox", context.resource_registry().lookup_name<resources::Material>(skybox).first));
    }

    template<class Archive>
    void load(Archive &archive) {
        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        archive(cereal::make_nvp("ambient_color", ambient_color));
        {
            std::string name;
            archive(cereal::make_nvp("skybox", name));
            skybox = context.resource_registry().get_resource_direct<resources::Material>(name);
        }
    }
};

} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializableSceneLighting)

#endif