#pragma once


#include <scene_serialization/scene_serialization.hpp>
#include <scene_set/components/basic.hpp>
#include <rendering/components/mesh_renderer.hpp>
#include <serialization/scene_set/components/basic.hpp>
#include <serialization/rendering/components/mesh_renderer.hpp>
#include <serialization/rendering/components/camera.hpp>


#include <nodec/resource_management/resource_registry.hpp>


class SceneSerializationModuleBackend : public scene_serialization::SceneSerialization {
    using SceneEntity = scene_set::SceneEntity;
    using SceneRegistry = scene_set::SceneRegistry;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    SceneSerializationModuleBackend(ResourceRegistry* pResourceRegistry)
        : mpResourceRegistry(pResourceRegistry) {
        using namespace rendering::components;
        using namespace rendering::resources;
        using namespace scene_set::components;


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
            [=](const SerializableMeshRenderer serializable, SceneEntity entity, SceneRegistry& registry) {
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

        register_component<Name, SerializableName>(
            [](const Name& name) {
                auto serializable_name = std::make_shared<SerializableName>();
                serializable_name->name = name.name;
                return serializable_name;
            },
            [](const SerializableName& serializable_name, SceneEntity entity, SceneRegistry& registry) {
                registry.emplace_component<Name>(entity);
                auto& name = registry.get_component<Name>(entity);
                name.name = serializable_name.name;
            });

        register_component<Transform, SerializableTransform>(
            [](const Transform& trfm) {
                auto serializable_trfm = std::make_shared<SerializableTransform>();
                serializable_trfm->local_position = trfm.local_position;
                serializable_trfm->local_scale = trfm.local_scale;
                serializable_trfm->local_rotation = trfm.local_rotation;
                return serializable_trfm;
            },
            [](const SerializableTransform& serializable_trfm, SceneEntity entity, SceneRegistry& registry) {
                registry.emplace_component<Transform>(entity);
                auto& trfm = registry.get_component<Transform>(entity);
                trfm.local_position = serializable_trfm.local_position;
                trfm.local_rotation = serializable_trfm.local_rotation;
                trfm.local_scale = serializable_trfm.local_scale;
                trfm.dirty = true;
            });

        register_component<Camera, SerializableCamera>(
            [](const Camera& camera) {
                auto serializable_camera = std::make_shared<SerializableCamera>();
                serializable_camera->farClipPlane = camera.farClipPlane;
                serializable_camera->nearClipPlane = camera.nearClipPlane;
                serializable_camera->fovAngle = camera.fovAngle;
                return serializable_camera;
            },
            [](const SerializableCamera& serializable_camera, SceneEntity entity, SceneRegistry& registry) {
                registry.emplace_component<Camera>(entity);
                auto& camera = registry.get_component<Camera>(entity);
                camera.farClipPlane = serializable_camera.farClipPlane;
                camera.nearClipPlane = serializable_camera.nearClipPlane;
                camera.fovAngle = serializable_camera.fovAngle;
            });
    }

private:
    ResourceRegistry* mpResourceRegistry;

};