#include <SceneSerialization/SceneSerializationModuleBackend.hpp>

#include <nodec_rendering/components/image_renderer.hpp>
#include <nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_rendering/components/non_visible.hpp>
#include <nodec_rendering/components/point_light.hpp>
#include <nodec_rendering/components/text_renderer.hpp>
#include <nodec_scene/components/name.hpp>
#include <nodec_scene/components/transform.hpp>
#include <nodec_scene/scene_registry.hpp>
#include <nodec_scene_audio/components/audio_source.hpp>
#include <nodec_scene_serialization/components/non_serialized.hpp>
#include <nodec_serialization/nodec_physics/components/physics_shape.hpp>
#include <nodec_serialization/nodec_physics/components/rigid_body.hpp>
#include <nodec_serialization/nodec_rendering/components/camera.hpp>
#include <nodec_serialization/nodec_rendering/components/directional_light.hpp>
#include <nodec_serialization/nodec_rendering/components/image_renderer.hpp>
#include <nodec_serialization/nodec_rendering/components/mesh_renderer.hpp>
#include <nodec_serialization/nodec_rendering/components/non_visible.hpp>
#include <nodec_serialization/nodec_rendering/components/point_light.hpp>
#include <nodec_serialization/nodec_rendering/components/post_processing.hpp>
#include <nodec_serialization/nodec_rendering/components/scene_lighting.hpp>
#include <nodec_serialization/nodec_rendering/components/text_renderer.hpp>
#include <nodec_serialization/nodec_scene/components/name.hpp>
#include <nodec_serialization/nodec_scene/components/transform.hpp>
#include <nodec_serialization/nodec_scene_audio/components/audio_source.hpp>

SceneSerializationModuleBackend::SceneSerializationModuleBackend(nodec::resource_management::ResourceRegistry *resource_registry) {
    using namespace nodec_rendering::components;
    using namespace nodec_rendering::resources;
    using namespace nodec_scene::components;
    using namespace nodec_scene_audio::components;
    using namespace nodec_physics::components;
    using namespace nodec_scene;

    register_component<MeshRenderer, SerializableMeshRenderer>(
        [=](const MeshRenderer &component) {
            auto serializable = std::make_unique<SerializableMeshRenderer>();

            for (auto &mesh : component.meshes) {
                auto name = resource_registry->lookup_name(mesh).first;
                serializable->meshes.push_back(name);
            }

            for (auto &material : component.materials) {
                auto name = resource_registry->lookup_name(material).first;
                serializable->materials.push_back(name);
            }

            return serializable;
        },
        [=](const SerializableMeshRenderer serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &component = registry.emplace_component<MeshRenderer>(entity).first;

            for (auto &name : serializable.meshes) {
                auto mesh = resource_registry->get_resource<Mesh>(name).get();
                component.meshes.push_back(mesh);
            }

            for (auto &name : serializable.materials) {
                auto material = resource_registry->get_resource<Material>(name).get();
                component.materials.push_back(material);
            }
        });

    register_component<ImageRenderer, SerializableImageRenderer>(
        [=](const ImageRenderer &renderer) {
            auto serializable = std::make_unique<SerializableImageRenderer>();
            serializable->image = renderer.image;
            serializable->material = renderer.material;
            serializable->pixels_per_unit = renderer.pixels_per_unit;
            return serializable;
        },
        [=](const SerializableImageRenderer &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &renderer = registry.emplace_component<ImageRenderer>(entity).first;
            renderer.image = serializable.image;
            renderer.material = serializable.material;
            renderer.pixels_per_unit = serializable.pixels_per_unit;
        });

    register_component<TextRenderer, SerializableTextRenderer>(
        [=](const TextRenderer &renderer) {
            auto serializable = std::make_unique<SerializableTextRenderer>();
            serializable->font = renderer.font;
            serializable->material = renderer.material;
            serializable->text = renderer.text;
            serializable->pixel_size = renderer.pixel_size;
            serializable->pixels_per_unit = renderer.pixels_per_unit;
            serializable->color = renderer.color;
            return serializable;
        },
        [=](const SerializableTextRenderer &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &renderer = registry.emplace_component<TextRenderer>(entity).first;
            renderer.font = serializable.font;
            renderer.material = serializable.material;
            renderer.text = serializable.text;
            renderer.pixel_size = serializable.pixel_size;
            renderer.pixels_per_unit = serializable.pixels_per_unit;
            renderer.color = serializable.color;
        });

    register_component<PostProcessing, SerializablePostProcessing>(
        [=](const PostProcessing &processing) {
            auto serializable = std::make_unique<SerializablePostProcessing>();

            for (const auto &effect : processing.effects) {
                const auto name = resource_registry->lookup_name<Material>(effect.material).first;
                serializable->effects.push_back({effect.enabled, name});
            }
            return serializable;
        },
        [=](const SerializablePostProcessing &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &processing = registry.emplace_component<PostProcessing>(entity).first;
            for (const auto &effect : serializable.effects) {
                processing.effects.push_back({effect.enabled, resource_registry->get_resource<Material>(effect.material).get()});
            }
        });

    register_component<Name, SerializableName>(
        [](const Name &name) {
            auto serializable_name = std::make_unique<SerializableName>();
            serializable_name->name = name.name;
            return serializable_name;
        },
        [](const SerializableName &serializable_name, SceneEntity entity, SceneRegistry &registry) {
            auto &name = registry.emplace_component<Name>(entity).first;
            name.name = serializable_name.name;
        });

    register_component<Transform, SerializableTransform>(
        [](const Transform &trfm) {
            auto serializable_trfm = std::make_unique<SerializableTransform>();
            serializable_trfm->local_position = trfm.local_position;
            serializable_trfm->local_scale = trfm.local_scale;
            serializable_trfm->local_rotation = trfm.local_rotation;
            return serializable_trfm;
        },
        [](const SerializableTransform &serializable_trfm, SceneEntity entity, SceneRegistry &registry) {
            auto &trfm = registry.emplace_component<Transform>(entity).first;
            trfm.local_position = serializable_trfm.local_position;
            trfm.local_rotation = serializable_trfm.local_rotation;
            trfm.local_scale = serializable_trfm.local_scale;
            trfm.dirty = true;
        });

    register_component<Camera, SerializableCamera>(
        [](const Camera &camera) {
            auto serializable_camera = std::make_unique<SerializableCamera>();
            serializable_camera->far_clip_plane = camera.far_clip_plane;
            serializable_camera->near_clip_plane = camera.near_clip_plane;
            serializable_camera->fov_angle = camera.fov_angle;
            serializable_camera->projection = camera.projection;
            serializable_camera->ortho_width = camera.ortho_width;
            return serializable_camera;
        },
        [](const SerializableCamera &serializable_camera, SceneEntity entity, SceneRegistry &registry) {
            auto &camera = registry.emplace_component<Camera>(entity).first;
            camera.far_clip_plane = serializable_camera.far_clip_plane;
            camera.near_clip_plane = serializable_camera.near_clip_plane;
            camera.fov_angle = serializable_camera.fov_angle;
            camera.projection = serializable_camera.projection;
            camera.ortho_width = serializable_camera.ortho_width;
        });

    register_component<DirectionalLight, SerializableDirectionalLight>(
        [](const DirectionalLight &light) {
            auto serializable_light = std::make_unique<SerializableDirectionalLight>();
            serializable_light->color = light.color;
            serializable_light->intensity = light.intensity;
            return serializable_light;
        },
        [](const SerializableDirectionalLight &serializable_light, SceneEntity entity, SceneRegistry &registry) {
            auto &light = registry.emplace_component<DirectionalLight>(entity).first;
            light.color = serializable_light.color;
            light.intensity = serializable_light.intensity;
        });

    register_component<PointLight, SerializablePointLight>(
        [](const PointLight &light) {
            auto serializable = std::make_unique<SerializablePointLight>();
            serializable->color = light.color;
            serializable->intensity = light.intensity;
            serializable->range = light.range;
            return serializable;
        },
        [](const SerializablePointLight &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &light = registry.emplace_component<PointLight>(entity).first;
            light.color = serializable.color;
            light.intensity = serializable.intensity;
            light.range = serializable.range;
        });

    register_component<SceneLighting, SerializableSceneLighting>(
        [](const SceneLighting &lighting) {
            auto serializable = std::make_unique<SerializableSceneLighting>();
            serializable->ambient_color = lighting.ambient_color;
            return serializable;
        },
        [](const SerializableSceneLighting &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &lighting = registry.emplace_component<SceneLighting>(entity).first;
            lighting.ambient_color = serializable.ambient_color;
        });

    register_component<AudioSource, SerializableAudioSource>(
        [](const AudioSource &source) {
            auto serializable = std::make_unique<SerializableAudioSource>();
            serializable->clip = source.clip;
            serializable->is_playing = source.is_playing;
            serializable->loop = source.loop;
            serializable->position = source.position;
            serializable->volume = source.volume;
            return serializable;
        },
        [](const SerializableAudioSource &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &source = registry.emplace_component<AudioSource>(entity).first;
            source.clip = serializable.clip;
            source.is_playing = serializable.is_playing;
            source.loop = serializable.loop;
            source.position = serializable.position;
            source.volume = serializable.volume;
        });

    register_component<NonVisible, SerializableNonVisible>(
        [](const NonVisible &non_visible) {
            auto serializable = std::make_unique<SerializableNonVisible>();
            serializable->self = non_visible.self;
            return serializable;
        },
        [](const SerializableNonVisible &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &non_visible = registry.emplace_component<NonVisible>(entity).first;
            non_visible.self = serializable.self;
        });

    register_component<RigidBody, SerializableRigidBody>(
        [](const RigidBody &body) {
            auto serializable = std::make_unique<SerializableRigidBody>();
            serializable->mass = body.mass;
            return serializable;
        },
        [](const SerializableRigidBody &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &body = registry.emplace_component<RigidBody>(entity).first;
            body.mass = serializable.mass;
        });
    
    register_component<PhysicsShape, SerializablePhysicsShape>(
        [](const PhysicsShape &shape) {
            auto serializable = std::make_unique<SerializablePhysicsShape>();
            serializable->shape_type = shape.shape_type;
            serializable->size = shape.size;
            serializable->radius = shape.radius;
            return serializable;
        },
        [](const SerializablePhysicsShape &serializable, SceneEntity entity, SceneRegistry &registry) {
            auto &shape = registry.emplace_component<PhysicsShape>(entity).first;
            shape.shape_type = serializable.shape_type;
            shape.size = serializable.size;
            shape.radius = serializable.radius;
        });
}
