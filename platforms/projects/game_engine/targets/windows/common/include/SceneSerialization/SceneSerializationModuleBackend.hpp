#pragma once

#include <scene_serialization/scene_serialization.hpp>
#include <scene_set/components/basic.hpp>

#include <rendering/components/mesh_renderer.hpp>

#include <nodec/resource_management/resource_registry.hpp>


class SerializableMeshRenderer : public scene_serialization::BaseSerializableComponent {
    using MeshRenderer = rendering::components::MeshRenderer;

public:

    std::vector<std::string> meshes;
    std::vector<std::string> materials;


    template<class Archive>
    void serialize(Archive& archive) {
        archive(meshes, materials);
    }
};




class SceneSerializationModuleBackend : public scene_serialization::SceneSerialization {

    using MeshRenderer = rendering::components::MeshRenderer;

public:
    SceneSerializationModuleBackend() {

        //register_serialization<MeshRenderer, SerializableMeshRenderer>(1, 2);


    }
};