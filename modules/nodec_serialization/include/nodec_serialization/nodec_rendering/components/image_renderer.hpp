#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__IMAGE_RENDERER_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__IMAGE_RENDERER_HPP_

#include <nodec_scene_serialization/scene_serialization.hpp>

#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializableImageRenderer : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableImageRenderer()
        : BaseSerializableComponent(this) {}

    std::string image;
    std::string material;

    int pixelsPerUnit{100};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("image", image));
        archive(cereal::make_nvp("material", material));
        archive(cereal::make_nvp("pixelsPerUnit", pixelsPerUnit));
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableImageRenderer);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableImageRenderer);

#endif