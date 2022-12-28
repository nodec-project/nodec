#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>

#include <cereal/types/vector.hpp>

namespace nodec_rendering {
namespace components {

class SerializableMeshRenderer : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableMeshRenderer()
        : BaseSerializableComponent(this) {
    }

    std::vector<std::string> meshes;
    std::vector<std::string> materials;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("meshes", meshes));
        archive(cereal::make_nvp("materials", materials));
    }
};

} // namespace components
} // namespace nodec_rendering

CEREAL_REGISTER_TYPE(nodec_rendering::components::SerializableMeshRenderer)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, nodec_rendering::components::SerializableMeshRenderer)

#endif