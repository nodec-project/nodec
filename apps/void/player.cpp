#include "player.hpp"


using namespace nodec;
using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_modules::input::keyboard::interfaces;
using namespace nodec_modules::input::mouse::interfaces;
using namespace nodec_modules::screen::interfaces;

void Player::on_awake()
{
    enable_frame_update();
}

void Player::on_frame_start(Rendering& rendering)
{
    if (auto camera = App::main_camera.lock())
    {
        logging::debug("as", __FILE__, __LINE__);
        rendering.regist_camera(camera);
    }

    scene_object().transform().local_position.y = 1.6; // height
}


void Player::on_frame_update(Rendering& rendering)
{
    Keyboard& keyboard = get_engine()->keyboard();
    Mouse& mouse = get_engine()->mouse();
    Screen& screen = get_engine()->screen();

    float delta_time = rendering.frame_delta_time();

    float speed = 0.5f;


    if (keyboard.get_key_pressed(Key::LeftShift))
    {
        speed *= 2.0f;
    }

    // --- locomotion ---
    Vector3f forward = math::gfx::transform(Vector3f(0, 0, 1), scene_object().transform().local_rotation); forward.y = 0;
    forward = math::normalize(forward);

    Vector3f right = math::gfx::transform(Vector3f(1, 0, 0), scene_object().transform().local_rotation); right.y = 0;
    right = math::normalize(right);

    Vector2f move_vec;

    if (keyboard.get_key_pressed(Key::A))
    {
        move_vec.x = -1.0f;
    }

    if (keyboard.get_key_pressed(Key::D))
    {
        move_vec.x = 1.0f;
    }

    if (keyboard.get_key_pressed(Key::W))
    {
        move_vec.y = 1.0f;
    }

    if (keyboard.get_key_pressed(Key::S))
    {
        move_vec.y = -1.0f;
    }
    if (math::norm(move_vec) > 0.001f)
    {
        move_vec = math::normalize(move_vec);

        scene_object().transform().local_position += move_vec.y * forward * speed * delta_time;
        scene_object().transform().local_position += move_vec.x * right * speed * delta_time;
    }

    // end locomotion ---

    // --- angle ---
    if (keyboard.get_key_pressed(Key::J))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(0, -1, 0));
        scene_object().transform().local_rotation *= delta;
    }
    if (keyboard.get_key_pressed(Key::L))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(0, 1, 0));
        scene_object().transform().local_rotation *= delta;
    }

    if (keyboard.get_key_pressed(Key::I))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(-1, 0, 0));
        scene_object().transform().local_rotation *= delta;
    }
    if (keyboard.get_key_pressed(Key::K))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(1, 0, 0));
        scene_object().transform().local_rotation *= delta;
    }
    // end angle ---
}
