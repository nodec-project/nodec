#pragma once

#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_scene/components/basic.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_serialization/nodec_rendering/components/camera.hpp>
#include <nodec_serialization/nodec_rendering/components/directional_light.hpp>
#include <nodec_serialization/nodec_rendering/components/image_renderer.hpp>
#include <nodec_serialization/nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_serialization/nodec_rendering/components/point_light.hpp>
#include <nodec_serialization/nodec_rendering/components/post_processing.hpp>
#include <nodec_serialization/nodec_rendering/components/scene_lighting.hpp>
#include <nodec_serialization/nodec_scene/components/basic.hpp>

#include <nodec/resource_management/resource_registry.hpp>

class SceneSerializationModuleBackend : public nodec_scene_serialization::SceneSerialization {
    using SceneEntity = nodec_scene::SceneEntity;
    using SceneRegistry = nodec_scene::SceneRegistry;
    using ResourceRegistry = nodec::resource_management::ResourceRegistry;

public:
    SceneSerializationModuleBackend(ResourceRegistry *pResourceRegistry)
        : mpResourceRegistry(pResourceRegistry) {
        using namespace nodec_rendering::components;
        using namespace nodec_rendering::resources;
        using namespace nodec_scene::components;

        register_component<MeshRenderer, SerializableMeshRenderer>(
            [=](const MeshRenderer &component) {
                auto serializable = std::make_shared<SerializableMeshRenderer>();

                for (auto &mesh : component.meshes) {
                    auto name = mpResourceRegistry->lookup_name(mesh).first;
                    serializable->meshes.push_back(name);
                }

                for (auto &material : component.materials) {
                    auto name = mpResourceRegistry->lookup_name(material).first;
                    serializable->materials.push_back(name);
                }

                return serializable;
            },
            [=](const SerializableMeshRenderer serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<MeshRenderer>(entity);
                auto &component = registry.get_component<MeshRenderer>(entity);

                for (auto &name : serializable.meshes) {
                    auto mesh = mpResourceRegistry->get_resource<Mesh>(name).get();
                    component.meshes.push_back(mesh);
                }

                for (auto &name : serializable.materials) {
                    auto material = mpResourceRegistry->get_resource<Material>(name).get();
                    component.materials.push_back(material);
                }
            });

        register_component<ImageRenderer, SerializableImageRenderer>(
            [=](const ImageRenderer &renderer) {
                auto serializable = std::make_shared<SerializableImageRenderer>();
                serializable->image = mpResourceRegistry->lookup_name<Texture>(renderer.image).first;
                serializable->material = mpResourceRegistry->lookup_name<Material>(renderer.material).first;
                serializable->pixelsPerUnit = renderer.pixelsPerUnit;
                return serializable;
            },
            [=](const SerializableImageRenderer &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<ImageRenderer>(entity);
                auto &renderer = registry.get_component<ImageRenderer>(entity);
                renderer.image = mpResourceRegistry->get_resource<Texture>(serializable.image).get();
                renderer.material = mpResourceRegistry->get_resource<Material>(serializable.material).get();
                renderer.pixelsPerUnit = serializable.pixelsPerUnit;
            });

        register_component<PostProcessing, SerializablePostProcessing>(
            [=](const PostProcessing &processing) {
                auto serializable = std::make_shared<SerializablePostProcessing>();

                for (const auto& effect : processing.effects) {
                    const auto name = mpResourceRegistry->lookup_name<Material>(effect.material).first;
                    serializable->effects.push_back({effect.enabled, name});
                }
                return serializable;
            },
            [=](const SerializablePostProcessing &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<PostProcessing>(entity);
                auto &processing = registry.get_component<PostProcessing>(entity);
                for (const auto& effect : serializable.effects) {
                    processing.effects.push_back({effect.enabled, mpResourceRegistry->get_resource<Material>(effect.material).get()});
                }
            });

        register_component<Name, SerializableName>(
            [](const Name &name) {
                auto serializable_name = std::make_shared<SerializableName>();
                serializable_name->name = name.name;
                return serializable_name;
            },
            [](const SerializableName &serializable_name, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<Name>(entity);
                auto &name = registry.get_component<Name>(entity);
                name.name = serializable_name.name;
            });

        register_component<Transform, SerializableTransform>(
            [](const Transform &trfm) {
                auto serializable_trfm = std::make_shared<SerializableTransform>();
                serializable_trfm->local_position = trfm.local_position;
                serializable_trfm->local_scale = trfm.local_scale;
                serializable_trfm->local_rotation = trfm.local_rotation;
                return serializable_trfm;
            },
            [](const SerializableTransform &serializable_trfm, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<Transform>(entity);
                auto &trfm = registry.get_component<Transform>(entity);
                trfm.local_position = serializable_trfm.local_position;
                trfm.local_rotation = serializable_trfm.local_rotation;
                trfm.local_scale = serializable_trfm.local_scale;
                trfm.dirty = true;
            });

        register_component<Camera, SerializableCamera>(
            [](const Camera &camera) {
                auto serializable_camera = std::make_shared<SerializableCamera>();
                serializable_camera->farClipPlane = camera.farClipPlane;
                serializable_camera->nearClipPlane = camera.nearClipPlane;
                serializable_camera->fovAngle = camera.fovAngle;
                return serializable_camera;
            },
            [](const SerializableCamera &serializable_camera, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<Camera>(entity);
                auto &camera = registry.get_component<Camera>(entity);
                camera.farClipPlane = serializable_camera.farClipPlane;
                camera.nearClipPlane = serializable_camera.nearClipPlane;
                camera.fovAngle = serializable_camera.fovAngle;
            });

        register_component<DirectionalLight, SerializableDirectionalLight>(
            [](const DirectionalLight &light) {
                auto serializable_light = std::make_shared<SerializableDirectionalLight>();
                serializable_light->color = light.color;
                serializable_light->intensity = light.intensity;
                return serializable_light;
            },
            [](const SerializableDirectionalLight &serializable_light, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<DirectionalLight>(entity);
                auto &light = registry.get_component<DirectionalLight>(entity);
                light.color = serializable_light.color;
                light.intensity = serializable_light.intensity;
            });

        register_component<PointLight, SerializablePointLight>(
            [](const PointLight &light) {
                auto serializable = std::make_shared<SerializablePointLight>();
                serializable->color = light.color;
                serializable->intensity = light.intensity;
                serializable->range = light.range;
                return serializable;
            },
            [](const SerializablePointLight &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<PointLight>(entity);
                auto &light = registry.get_component<PointLight>(entity);
                light.color = serializable.color;
                light.intensity = serializable.intensity;
                light.range = serializable.range;
            });

        register_component<SceneLighting, SerializableSceneLighting>(
            [](const SceneLighting &lighting) {
                auto serializable = std::make_shared<SerializableSceneLighting>();
                serializable->ambient_color = lighting.ambient_color;
                return serializable;
            },
            [](const SerializableSceneLighting &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<SceneLighting>(entity);
                auto &lighting = registry.get_component<SceneLighting>(entity);
                lighting.ambient_color = serializable.ambient_color;
            });
    }

private:
    ResourceRegistry *mpResourceRegistry;
};