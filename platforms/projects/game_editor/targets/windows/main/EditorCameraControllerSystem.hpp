#pragma once

#include <nodec_rendering/components/main_camera.hpp>
#include <nodec_scene/scene_registry.hpp>
#include <nodec_input/keyboard/keyboard.hpp>
#include <nodec_input/mouse/mouse.hpp>

struct EditorCameraController {
};

class EditorCameraControllerSystem {
public:
    void setup(nodec_scene::SceneRegistry &scene_registry) {
        using namespace nodec_rendering::components;
        using namespace nodec_scene;

        scene_registry.component_constructed<MainCamera>().connect([](SceneRegistry &registry, SceneEntity entt) {
            registry.emplace_component<EditorCameraController>(entt);
        });

        scene_registry.component_destroyed<MainCamera>().connect([](SceneRegistry &registry, SceneEntity entt) {
            registry.remove_component<EditorCameraController>(entt);
        });
    }

    void activate() {

    }

    void deactivate() {
        
    }

    void update(nodec_scene::SceneRegistry &scene_registry) {
    }

private:
    bool active_{false};
};