#ifndef NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_
#define NODEC_SERIALIZATION__NODEC_RENDERING__COMPONENTS__MESH_RENDERER_HPP_

#include <cereal/types/vector.hpp>

#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_scene_serialization/serializable_component.hpp>

namespace nodec_rendering {
namespace components {

class SerializableMeshRenderer : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableMeshRenderer()
        : BaseSerializableComponent(this) {
    }

    SerializableMeshRenderer(const MeshRenderer &other)
        : BaseSerializableComponent(this),
          meshes(other.meshes),
          materials(other.materials) {}

    operator MeshRenderer() const noexcept {
        MeshRenderer value;
        value.meshes = meshes;
        value.materials = materials;
        return value;
    }

    std::vector<std::shared_ptr<resources::Mesh>> meshes;
    std::vector<std::shared_ptr<resources::Material>> materials;

    template<class Archive>
    void save(Archive &archive) const {
        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        std::vector<std::string> names;

        for (const auto &mesh : meshes) {
            names.push_back(context.resource_registry().lookup_name<resources::Mesh>(mesh).first);
        }
        archive(cereal::make_nvp("meshes", names));

        names.clear();

        for (const auto &material : materials) {
            names.push_back(context.resource_registry().lookup_name<resources::Material>(material).first);
        }
        archive(cereal::make_nvp("materials", names));
    }

    template<class Archive>
    void load(Archive &archive) {
        meshes.clear();
        materials.clear();

        using namespace nodec_scene_serialization;
        ArchiveContext &context = cereal::get_user_data<ArchiveContext>(archive);

        std::vector<std::string> names;

        archive(cereal::make_nvp("meshes", names));
        for (const auto &name : names) {
            meshes.push_back(context.resource_registry().get_resource_direct<resources::Mesh>(name));
        }

        names.clear();
        archive(cereal::make_nvp("materials", names));
        for (const auto &name : names) {
            materials.push_back(context.resource_registry().get_resource_direct<resources::Material>(name));
        }
    }
};

} // namespace components
} // namespace nodec_rendering

NODEC_SCENE_REGISTER_SERIALIZABLE_COMPONENT(nodec_rendering::components::SerializableMeshRenderer)

#endif