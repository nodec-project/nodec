#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__TEXT_RENDERER_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__TEXT_RENDERER_HPP_

#include <nodec_scene_serialization/base_serializable_component.hpp>

#include <nodec_rendering/resources/font.hpp>
#include <nodec_rendering/resources/material.hpp>

#include <nodec_serialization/nodec/vector4.hpp>

namespace nodec_rendering {
namespace components {

class SerializableTextRenderer : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableTextRenderer()
        : BaseSerializableComponent(this) {}

    std::string text;
    std::shared_ptr<resources::Font> font;
    std::shared_ptr<resources::Material> material;

    int pixel_size{10};
    int pixels_per_unit{10};

    nodec::Vector4f color{1.0f, 1.0f, 1.0f, 1.0f};

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        archive(cereal::make_nvp("text", text));
        archive(cereal::make_nvp("font", context.resource_registry().lookup_name<resources::Font>(font).first));
        archive(cereal::make_nvp("material", context.resource_registry().lookup_name<resources::Material>(material).first));
        archive(cereal::make_nvp("pixel_size", pixel_size));
        archive(cereal::make_nvp("pixels_per_unit", pixels_per_unit));
        archive(cereal::make_nvp("color", color));
    }

    template<class Archive>
    void load(Archive &archive) {
        using namespace nodec_scene_serialization;
        using namespace nodec::resource_management;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        archive(cereal::make_nvp("text", text));

        {
            std::string name;
            archive(cereal::make_nvp("font", name));
            font = context.resource_registry().get_resource<resources::Font>(name, LoadPolicy::Direct).get();
        }
        {
            std::string name;
            archive(cereal::make_nvp("material", name));
            material = context.resource_registry().get_resource<resources::Material>(name, LoadPolicy::Direct).get();
        }

        archive(cereal::make_nvp("pixel_size", pixel_size));
        archive(cereal::make_nvp("pixels_per_unit", pixels_per_unit));
        archive(cereal::make_nvp("color", color));
    }
};
} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableTextRenderer)

CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableTextRenderer)

#endif