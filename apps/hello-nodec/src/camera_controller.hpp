#ifndef CAMERA_CONTROLLER_HPP_
#define CAMERA_CONTROLLER_HPP_

#include "app.hpp"

struct CameraController {
    float speed{1.0f};
};

class SerializableCameraController : public nodec_scene_serialization::BaseSerializableComponent {
public:
    SerializableCameraController()
        : BaseSerializableComponent{this} {
    }

    float speed{1.0f};

    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::make_nvp("speed", speed));
    }
};

CEREAL_REGISTER_TYPE(SerializableCameraController);
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, SerializableCameraController);

class CameraControllerSystem {
public:
    CameraControllerSystem(nodec_scene::Scene &scene,
                           nodec_input::Input &input,
                           nodec_scene_serialization::SceneSerialization &serialization) {
        using namespace nodec;
        using namespace nodec_scene;
        using namespace nodec_input::keyboard;
        using namespace nodec_input::mouse;

        scene.stepped().connect([&](nodec_scene::Scene &scene) { on_stepped(scene); });
        input.keyboard().key_event().connect([&](const nodec_input::keyboard::KeyEvent &event) {
            // logging::InfoStream(__FILE__, __LINE__) << event;
            if (event.key == Key::W) {
                w_pressed = (event.type == KeyEvent::Type::Press);
            }
            if (event.key == Key::A) {
                a_pressed = (event.type == KeyEvent::Type::Press);
            }
            if (event.key == Key::S) {
                s_pressed = (event.type == KeyEvent::Type::Press);
            }
            if (event.key == Key::D) {
                d_pressed = (event.type == KeyEvent::Type::Press);
            }
            // logging::InfoStream(__FILE__, __LINE__) << w_pressed << s_pressed << a_pressed << d_pressed;
        });

        input.mouse().mouse_event().connect([&](const nodec_input::mouse::MouseEvent &event) {
            // logging::InfoStream(__FILE__, __LINE__) << event;
            static Vector2i prev_pos;

            if (event.type == MouseEvent::Type::Press && event.button == MouseButton::Right) {
                prev_pos = event.position;
            }
            if (event.type == MouseEvent::Type::Move && (event.buttons & MouseButton::Right)) {
                rotation_delta += event.position - prev_pos;
                prev_pos = event.position;
            }
        });

        serialization.register_component<CameraController, SerializableCameraController>(
            [&](const CameraController &controller) { return std::make_shared<SerializableCameraController>(); },
            [&](const const SerializableCameraController &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<CameraController>(entity);
            });
    }

#ifdef EDITOR_MODE
    static void setup_editor(scene_editor::SceneEditor &editor) {
        editor.inspector_component_registry().register_component<CameraController>(
            "Camera Controller",
            [](CameraController &ctrl) {
                ImGui::DragFloat("Speed", &ctrl.speed);
            });
    }
#endif

private:
    void on_stepped(nodec_scene::Scene &scene) {
        using namespace nodec;
        using namespace nodec_scene;
        using namespace nodec_scene::components;
        using namespace nodec_rendering::components;

        scene.registry().view<Camera, Transform, CameraController>().each([&](const SceneEntity &entity, Camera &camera, Transform &trfm, CameraController &ctrl) {
            // TODO: Get delta time from scene.
            const float delta_time = 1.0f / 60;

            auto forward = math::gfx::transform(Vector3f(0, 0, 1), trfm.local_rotation);
            auto right = math::gfx::transform(Vector3f(1, 0, 0), trfm.local_rotation);

            Vector2f move_vec;

            if (w_pressed) move_vec.y += 1;
            if (s_pressed) move_vec.y -= 1;
            if (a_pressed) move_vec.x -= 1;
            if (d_pressed) move_vec.x += 1;
            if (math::norm(move_vec) > 0.001f) {
                logging::InfoStream(__FILE__, __LINE__) << move_vec;
                move_vec = math::normalize(move_vec);
                logging::InfoStream(__FILE__, __LINE__) << trfm.local_position;

                trfm.local_position += move_vec.y * forward * ctrl.speed * delta_time;
                trfm.local_position += move_vec.x * right * ctrl.speed * delta_time;

                logging::InfoStream(__FILE__, __LINE__) << trfm.local_position;
                trfm.dirty = true;
            }

            if (math::norm(rotation_delta) > 1) {
                logging::InfoStream(__FILE__, __LINE__) << rotation_delta;

                trfm.local_rotation *= math::gfx::angle_axis(rotation_delta.x * 0.1f, Vector3f(0, 1, 0));
                trfm.local_rotation *= math::gfx::angle_axis(rotation_delta.y * 0.1f, Vector3f(1, 0, 0));
                trfm.dirty = true;
                rotation_delta.set(0, 0);
            }
        });
    }

private:
    bool w_pressed{false};
    bool a_pressed{false};
    bool s_pressed{false};
    bool d_pressed{false};

    nodec::Vector2i rotation_delta;
};

#endif