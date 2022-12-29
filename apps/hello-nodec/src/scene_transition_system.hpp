#ifndef SCENE_TRANSITION_SYSTEM_HPP_
#define SCENE_TRANSITION_SYSTEM_HPP_

#include "app.hpp"

struct SceneTransition {
    std::vector<std::string> scenes;
    int current_scene_index{-1};
    nodec::optional<nodec_scene_serialization::SceneLoader::AsyncOperation> operation;
    bool dirty{false};
};

class SerializableSceneTransition : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableSceneTransition()
        : BaseSerializableComponent(this) {}

    std::vector<std::string> scenes;
    int current_scene_index{-1};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("scenes", scenes),
                cereal::make_nvp("current_scene_index", current_scene_index));
    }
};

CEREAL_REGISTER_TYPE(SerializableSceneTransition)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, SerializableSceneTransition)

class SceneTransitionSystem {
public:
    SceneTransitionSystem(
        nodec_world::World &world,
        nodec_scene_serialization::SceneSerialization &serialization,
        nodec_scene_serialization::SceneLoader &loader)
        : scene_loader_{loader} {
        serialization.register_component<SceneTransition, SerializableSceneTransition>(
            [&](auto &transition) {
                auto serializable = std::make_unique<SerializableSceneTransition>();
                serializable->scenes = transition.scenes;
                serializable->current_scene_index = transition.current_scene_index;
                return serializable;
            },
            [&](auto &serializable, auto entity, auto &registry) {
                auto &transition = registry.emplace_component<SceneTransition>(entity).first;
                transition.scenes = serializable.scenes;
                transition.current_scene_index = serializable.current_scene_index;
                transition.dirty = true;
            });

        world.stepped().connect([&](auto &world) { on_stepped(world); });
    }

#ifdef EDITOR_MODE
    static void setup_editor(nodec_scene_editor::SceneEditor &editor) {
        using namespace nodec;

        editor.inspector_component_registry().register_component<SceneTransition>(
            "Scene Transition",
            [](SceneTransition &transition) {
                int current_min = transition.scenes.size() > 0 ? 0 : -1;
                int current_max = transition.scenes.size() - 1;

                int new_index = transition.current_scene_index;

                if (ImGui::DragInt("Current Scene Index", &new_index, 0.1f)) {
                    new_index = nodec::clamp(new_index, current_min, current_max);
                    if (new_index != transition.current_scene_index) {
                        transition.current_scene_index = new_index;
                        transition.dirty = true;
                    }
                }

                imessentials::list_edit(
                    "Scenes", transition.scenes,
                    [&](int index, auto &name) {
                        auto &buffer = imessentials::get_text_buffer(1024, name);
                        if (ImGui::InputText("##name", buffer.data(), buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
                            name = buffer.data();
                            transition.dirty = true;
                        }
                    },
                    [&]() {
                        transition.scenes.emplace_back();
                    },
                    [&](int index, auto &name) {
                        transition.dirty = true;
                    });
            });
    }
#endif

private:
    void on_stepped(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec::entities;
        using namespace nodec_scene::components;

        world.scene().registry().view<SceneTransition, Hierarchy>().each([&](auto entt, SceneTransition &transition, Hierarchy &) {
            if (transition.dirty) {
                if (0 <= transition.current_scene_index && transition.current_scene_index < transition.scenes.size()) {
                    transition.operation = scene_loader_.load_async(transition.scenes[transition.current_scene_index], entt);

                    world.scene().hierarchy_system().remove_all_children(entt);
                } else {
                    logging::ErrorStream(__FILE__, __LINE__) << "scene index is out of range.";
                }
                transition.dirty = false;
            }

            if (transition.operation) {
                if (transition.operation->is_done()) {
                    logging::InfoStream(__FILE__, __LINE__) << "scene loaded";
                    transition.operation.reset();
                }
            }
        });
    }

private:
    nodec_scene_serialization::SceneLoader &scene_loader_;

    NODEC_DISABLE_COPY(SceneTransitionSystem)
};

#endif
