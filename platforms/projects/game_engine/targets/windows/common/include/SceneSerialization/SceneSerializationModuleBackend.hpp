#pragma once

#include "SerializableBasicComponents.hpp"

#include <scene_serialization/scene_serialization.hpp>
#include <scene_set/components/basic.hpp>

#include <rendering/components/mesh_renderer.hpp>

#include <nodec/resource_management/resource_registry.hpp>


class SceneSerializationModuleBackend : public scene_serialization::SceneSerialization {
    using SceneRegistry = scene_set::SceneRegistry;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;
    using Mesh = rendering::resources::Mesh;
    using Material = rendering::resources::Material;
    using MeshRenderer = rendering::components::MeshRenderer;

public:
    SceneSerializationModuleBackend(ResourceRegistry* pResourceRegistry)
        : mpResourceRegistry(pResourceRegistry) {

        register_component<MeshRenderer, SerializableMeshRenderer>(
            [=](const MeshRenderer& component) {
                auto serializable = std::make_shared<SerializableMeshRenderer>();

                for (auto& mesh : component.meshes) {
                    auto name = mpResourceRegistry->lookup_name(mesh).first;
                    serializable->meshes.push_back(name);
                }

                for (auto& material : component.materials) {
                    auto name = mpResourceRegistry->lookup_name(material).first;
                    serializable->materials.push_back(name);
                }

                return serializable;
            },
            [=](const SerializableMeshRenderer serializable, auto entity, SceneRegistry& registry) {
                registry.emplace_component<MeshRenderer>(entity);
                auto& component = registry.get_component<MeshRenderer>(entity);

                for (auto& name : serializable.meshes) {
                    auto mesh = mpResourceRegistry->get_resource<Mesh>(name).get();
                    component.meshes.push_back(mesh);
                }

                for (auto& name : serializable.materials) {
                    auto material = mpResourceRegistry->get_resource<Material>(name).get();
                    component.materials.push_back(material);
                }
            });


    }

private:
    ResourceRegistry* mpResourceRegistry;

};