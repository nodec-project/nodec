#ifndef OBJECT_SPAWN_SYSTEM_HPP_
#define OBJECT_SPAWN_SYSTEM_HPP_

#include "app.hpp"

struct ObjectSpawner {
    std::string scene_name;
};

struct SerializableObjectSpawner : public nodec_scene_serialization::BaseSerializableComponent {
    SerializableObjectSpawner()
        : BaseSerializableComponent(this) {}
    
    std::string scene_name;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(cereal::make_nvp("scene_name", scene_name));
    }
};

CEREAL_REGISTER_TYPE(SerializableObjectSpawner);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, SerializableObjectSpawner);

/**
 * @brief F キーのトグルで, ObjectSpawner Component が持つシーンの読み込みと破棄を行う.
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
        nodec::resource_management::ResourceRegistry &resource_registry)
        : serialization_{serialization}, 
            resource_registry_{resource_registry} {
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
                auto serializable = std::make_shared<SerializableObjectSpawner>();
                serializable->scene_name = spawner.scene_name;
                return serializable;
            },
            [&](const SerializableObjectSpawner &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<ObjectSpawner>(entity);
                auto& spawner = registry.get_component<ObjectSpawner>(entity);
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
            }
        );
    }
    #endif

    void on_stepped(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec_scene::components;

        bool need_update{false};
        if (f_key_state == KeyState::PressDown) {
            f_key_state = KeyState::StillPressed;

            need_update = true;
        }

        if (!need_update) return;

        switch (next_spawn_state) {
            case SpawnState::Disappeared:
                logging::InfoStream(__FILE__, __LINE__) << "Object now disappeared";

                world.scene().registry().view<ObjectSpawner>().each([&](auto entt, ObjectSpawner &spawner){
                    const Hierarchy *hier = world.scene().registry().try_get_component<Hierarchy>(entt);
                    if (hier == nullptr) return;

                    for (auto& child : hier->children) {
                        world.scene().registry().destroy_entity(child);
                    }
                });

                next_spawn_state = SpawnState::Spawned;
                break;

            case SpawnState::Spawned:
                logging::InfoStream(__FILE__, __LINE__) << "Object now spawned";

                world.scene().registry().view<ObjectSpawner>().each([](auto entt, ObjectSpawner &spawner){

                });

                next_spawn_state = SpawnState::Disappeared;
                break;
        }
    }

private:
    SpawnState next_spawn_state{SpawnState::Spawned};
    KeyState f_key_state{KeyState::Released};
    nodec_scene_serialization::SceneSerialization& serialization_;
    nodec::resource_management::ResourceRegistry &resource_registry_;
};

#endif
