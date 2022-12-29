#ifndef LIGHT_PARTICLE_HPP_
#define LIGHT_PARTICLE_HPP_

#include "app.hpp"

#include <random>

class LightParticle {
public:
    LightParticle(nodec_world::World &world, nodec::resource_management::ResourceRegistry &resource_registry,
                  nodec_scene_serialization::SceneSerialization &serialization) {
        using namespace nodec;
        using namespace nodec_rendering::components;
        using namespace nodec_scene::components;
        using namespace nodec_scene_serialization;
        using namespace nodec_scene_serialization::components;
        using namespace nodec_animations;

        curve.add_keyframe(Keyframe{0, 0.0f});
        curve.add_keyframe(Keyframe{30 * 1000, 360.0f});
        curve.set_wrap_mode(WrapMode::Loop);

        world.initialized().connect([&](nodec_world::World &world) {
            center_entt = world.scene().create_entity("Particles");

            world.scene().registry().emplace_component<NonSerialized>(center_entt);
            std::uniform_real_distribution<float> random_float(0.0, 1.0); // random floats between [0.0, 1.0]
            std::default_random_engine generator;

            auto proto_light = resource_registry.get_resource<SerializableSceneGraph>("org.nodec.hello-nodec/scenes/particle-light.scene").get();

            constexpr int COUNT = 100;
            for (int i = 0; i < COUNT; ++i) {
                auto entt = world.scene().create_entity("light");
                SceneEntityEmplacer{proto_light, world.scene(), entt, serialization}.emplace_all();
                world.scene().hierarchy_system().append_child(center_entt, entt);
                auto &trfm = world.scene().registry().get_component<Transform>(entt);
                Vector3f vec{
                    random_float(generator) * 2.0f - 1.0f,
                    random_float(generator) * 2.0f - 1.0f,
                    random_float(generator) * 2.0f - 1.0f};
                vec = math::normalize(vec);
                vec *= random_float(generator) * 10.0f;
                trfm.local_position = vec;
                trfm.dirty = true;
            }
        });

        world.stepped().connect([&](nodec_world::World &world) {
            if (!world.scene().registry().is_valid(center_entt)) return;

            auto &trfm = world.scene().registry().get_component<Transform>(center_entt);
            trfm.local_rotation = math::gfx::quatenion_from_angle_axis(curve.evaluate(world.clock().current_time() * 1000).second, Vector3f{0.0f, 1.0f, 0.0f});
            trfm.dirty = true;
        });
    }

private:
    nodec_scene::SceneEntity center_entt{nodec::entities::null_entity};
    nodec_animations::AnimationCurve curve;
};

#endif