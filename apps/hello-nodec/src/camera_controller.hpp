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

CEREAL_REGISTER_TYPE(SerializableCameraController)
CEREAL_REGISTER_POLYMORPHIC_RELATION(nodec_scene_serialization::BaseSerializableComponent, SerializableCameraController)

class CameraControllerSystem {
public:
    CameraControllerSystem(nodec_world::World &world,
                           std::shared_ptr<nodec_input::keyboard::Keyboard> keyboard,
                           std::shared_ptr<nodec_input::mouse::Mouse> mouse,
                           nodec_scene_serialization::SceneSerialization &serialization) {
        using namespace nodec;
        using namespace nodec_scene;
        using namespace nodec_input::keyboard;
        using namespace nodec_input::mouse;

        world.stepped().connect([&](nodec_world::World &world) { on_stepped(world); });
        keyboard->key_event().connect([&](const nodec_input::keyboard::KeyEvent &event) {
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

        mouse->mouse_event().connect([&](const nodec_input::mouse::MouseEvent &event) {
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
            [&](const CameraController &controller) {
                auto serializable = std::make_unique<SerializableCameraController>();
                serializable->speed = controller.speed;
                return serializable;
            },
            [&](const SerializableCameraController &serializable, SceneEntity entity, SceneRegistry &registry) {
                registry.emplace_component<CameraController>(entity);
                auto &controller = registry.get_component<CameraController>(entity);
                controller.speed = serializable.speed;
            });
    }

#ifdef EDITOR_MODE
    static void setup_editor(nodec_scene_editor::SceneEditor &editor) {
        editor.inspector_component_registry().register_component<CameraController>(
            "Camera Controller",
            [](CameraController &ctrl) {
                ImGui::DragFloat("Speed", &ctrl.speed);
            });
    }
#endif

private:
    void on_stepped(nodec_world::World &world) {
        using namespace nodec;
        using namespace nodec_scene;
        using namespace nodec_scene::components;
        using namespace nodec_rendering::components;

        // logging::InfoStream(__FILE__, __LINE__) << world.clock().current_time() << ", " << world.clock().delta_time();

        world.scene().registry().view<Transform, CameraController>().each([&](const SceneEntity &entity, Transform &trfm, CameraController &ctrl) {
            const float delta_time = world.clock().delta_time();

            auto forward = math::gfx::rotate(Vector3f(0, 0, 1), trfm.local_rotation);
            auto right = math::gfx::rotate(Vector3f(1, 0, 0), trfm.local_rotation);

            Vector2f move_vec;

            if (w_pressed) move_vec.y += 1;
            if (s_pressed) move_vec.y -= 1;
            if (a_pressed) move_vec.x -= 1;
            if (d_pressed) move_vec.x += 1;
            if (math::norm(move_vec) > 0.001f) {
                // logging::InfoStream(__FILE__, __LINE__) << move_vec;
                move_vec = math::normalize(move_vec);
                // logging::InfoStream(__FILE__, __LINE__) << trfm.local_position;

                trfm.local_position += move_vec.y * forward * ctrl.speed * delta_time;
                trfm.local_position += move_vec.x * right * ctrl.speed * delta_time;

                // logging::InfoStream(__FILE__, __LINE__) << trfm.local_position;
                trfm.dirty = true;
            }

            if (math::norm(rotation_delta) > 1) {
                constexpr float SCALE_FACTOR = 0.1f;
                // logging::InfoStream(__FILE__, __LINE__) << rotation_delta;

                // Apply rotation around the local right vector after current rotation.
                trfm.local_rotation = math::gfx::quatenion_from_angle_axis(rotation_delta.y * SCALE_FACTOR, right) * trfm.local_rotation;

                // And apply rotation arround the world up vector.
                trfm.local_rotation = math::gfx::quatenion_from_angle_axis(rotation_delta.x * SCALE_FACTOR, Vector3f(0.f, 1.f, 0.f)) * trfm.local_rotation;

                //trfm.local_rotation *= math::gfx::quatenion_from_angle_axis(rotation_delta.x * 0.1f, Vector3f(0, 1, 0));
                //trfm.local_rotation *= math::gfx::quatenion_from_angle_axis(rotation_delta.y * 0.1f, Vector3f(1, 0, 0));
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