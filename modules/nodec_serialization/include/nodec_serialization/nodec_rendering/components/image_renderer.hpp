#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__IMAGE_RENDERER_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__IMAGE_RENDERER_HPP_

#include <nodec_scene_serialization/serializable_component.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializableImageRenderer : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableImageRenderer()
        : BaseSerializableComponent(this) {}

    std::shared_ptr<resources::Texture> image;
    std::shared_ptr<resources::Material> material;

    int pixels_per_unit{100};

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        archive(cereal::make_nvp("image", context.resource_registry().lookup_name<resources::Texture>(image).first));
        archive(cereal::make_nvp("material", context.resource_registry().lookup_name<resources::Material>(material).first));
        archive(cereal::make_nvp("pixels_per_unit", pixels_per_unit));
    }

    template<class Archive>
    void load(Archive &archive) {
        using namespace nodec_scene_serialization;
        using namespace nodec::resource_management;

        auto &context = cereal::get_user_data<ArchiveContext>(archive);

        {
            std::string name;
            archive(cereal::make_nvp("image", name));
            image = context.resource_registry().get_resource<resources::Texture>(name, LoadPolicy::Direct).get();
        }
        {
            std::string name;
            archive(cereal::make_nvp("material", name));
            material = context.resource_registry().get_resource<resources::Material>(name, LoadPolicy::Direct).get();
        }
        archive(cereal::make_nvp("pixels_per_unit", pixels_per_unit));
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableImageRenderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableImageRenderer)

#endif