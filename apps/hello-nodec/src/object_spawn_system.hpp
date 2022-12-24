#ifndef OBJECT_SPAWN_SYSTEM_HPP_
#define OBJECT_SPAWN_SYSTEM_HPP_

#include "app.hpp"

struct ObjectSpawner {
    std::string scene_name;
    nodec::optional<nodec_scene_serialization::SceneLoader::AsyncOperation> operation;
};


class SerializableObjectSpawner : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableObjectSpawner()
        : BaseSerializableComponent(this) {}

    std::string scene_name;

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("scene_name", scene_name));
    }
};

CEREAL_REGISTER_TYPE(SerializableObjectSpawner)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, SerializableObjectSpawner)

/**
 * @brief Load or destroy the scene of ObjectSpawner Component by toggling of F key.
 *
 */
class ObjectSpawnSystem {
    enum class KeyState {
        PressDown,
        StillPressed,
        Released
    };

    enum class SpawnState {
        Spawned,
        Disappeared
    };

public:
    ObjectSpawnSystem(
        std::shared_ptr<nodec_input::keyboard::Keyboard> keyboard,
        nodec_world::World &world,
        nodec_scene_serialization::SceneSerialization &serialization,
        nodec_scene_serialization::SceneLoader &scene_loader)
        : serialization_{serialization},
          scene_loader_{scene_loader} {
        using namespace nodec_input::keyboard;
        using namespace nodec_scene;

        keyboard->key_event().connect([&](const KeyEvent &event) {
            if (event.key == Key::F) {
                switch (event.type) {
                case KeyEvent::Type::Press:
                    if (f_key_state == KeyState::Released) {
                        f_key_state = KeyState::PressDown;
                    }
                    break;

                case KeyEvent::Type::Release:
                    f_key_state = KeyState::Released;

                    break;
                }
            }
        });
        world.stepped().connect([&](nodec_world::World &world) { on_stepped(world); });

        serialization.register_component<ObjectSpawner, SerializableObjectSpawner>(
            [&](const ObjectSpawner &spawner) {
                auto serializable = std::make_unique<SerializableObjectSpawner>();
                serializable->scene_name = spawner.scene_name;
                return serializable;
            },
            [&](const SerializableObjectSpawner &serializable, SceneEntity entity, SceneRegistry &registry) {
                auto &spawner = registry.emplace_component<ObjectSpawner>(entity).first;
                spawner.scene_name = serializable.scene_name;
            });
    }

#ifdef EDITOR_MODE
    static void setup_editor(nodec_scene_editor::SceneEditor &editor) {
        editor.inspector_component_registry().register_component<ObjectSpawner>(
            "Object Spawner",
            [](ObjectSpawner &spawner) {
                auto &buffer = imessentials::get_text_buffer(1024, spawner.scene_name);

                if (ImGui::InputText("Scene Name", buffer.data(), buffer.size())) {
                }

                spawner.scene_name = buffer.data();
            });
    }
#endif

    void on_stepped(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec::entities;
        using namespace nodec_scene::components;
        using namespace nodec_scene_serialization;

        bool need_update{false};
        if (f_key_state == KeyState::PressDown) {
            f_key_state = KeyState::StillPressed;

            need_update = true;
        }

        world.scene().registry().view<ObjectSpawner>().each([&](auto entt, ObjectSpawner &spawner) {
            if (!spawner.operation) return;

            logging::InfoStream(__FILE__, __LINE__) << spawner.operation->progress();
            if (spawner.operation->is_done()) {
                spawner.operation.reset();
            }
        });

        if (!need_update) return;

        switch (next_spawn_state) {
        case SpawnState::Disappeared:
            logging::InfoStream(__FILE__, __LINE__) << "Object now disappeared";

            world.scene().registry().view<ObjectSpawner, Hierarchy>().each([&](auto entt, ObjectSpawner &spawner, Hierarchy&) {
                world.scene().hierarchy_system().remove_all_children(entt);
            });

            next_spawn_state = SpawnState::Spawned;
            break;

        case SpawnState::Spawned:
            logging::InfoStream(__FILE__, __LINE__) << "Object now spawned";

            world.scene().registry().view<ObjectSpawner>().each([&](auto entt, ObjectSpawner &spawner) {
                if (!spawner.operation) {
                    spawner.operation = SceneLoader::AsyncOperation(scene_loader_.load_async(spawner.scene_name, entt));
                }
            });

            next_spawn_state = SpawnState::Disappeared;
            break;
        }
    }

private:
    SpawnState next_spawn_state{SpawnState::Spawned};
    KeyState f_key_state{KeyState::Released};
    nodec_scene_serialization::SceneSerialization &serialization_;
    nodec_scene_serialization::SceneLoader &scene_loader_;
};

#endif
