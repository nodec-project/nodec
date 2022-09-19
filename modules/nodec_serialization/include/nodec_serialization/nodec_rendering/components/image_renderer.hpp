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

    // std::string image;
    std::shared_ptr<resources::Texture> image;

    // std::string material;
    std::shared_ptr<resources::Material> material;

    int pixelsPerUnit{100};

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<nodec_scene_serialization::ArchiveContext>(archive);

        archive(cereal::make_nvp("image", context.resource_registry().lookup_name<resources::Texture>(image).first));
        archive(cereal::make_nvp("material", context.resource_registry().lookup_name<resources::Material>(material).first));
        archive(cereal::make_nvp("pixelsPerUnit", pixelsPerUnit));
    }

    template<class Archive>
    void load(Archive &archive) {
        auto &context = cereal::get_user_data<nodec_scene_serialization::ArchiveContext>(archive);

        {
            std::string name;
            archive(cereal::make_nvp("image", name));
            image = context.resource_registry().get_resource<resources::Texture>(name, nodec::resource_management::LoadPolicy::Direct).get();
        }
        {
            std::string name;
            archive(cereal::make_nvp("material", name));
            material = context.resource_registry().get_resource<resources::Material>(name, nodec::resource_management::LoadPolicy::Direct).get();
        }
        archive(cereal::make_nvp("pixelsPerUnit", pixelsPerUnit));
    }
private:
    void rtti() {}
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableImageRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableImageRenderer);

#endif