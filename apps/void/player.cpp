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
    if (keyboard.get_key_pressed(Key::A))
    {
        scene_object().transform().local_position.x -= speed * delta_time;
    }

    if (keyboard.get_key_pressed(Key::D))
    {
        scene_object().transform().local_position.x += speed * delta_time;
    }

    if (keyboard.get_key_pressed(Key::W))
    {
        scene_object().transform().local_position.z += speed * delta_time;
    }

    if (keyboard.get_key_pressed(Key::S))
    {
        scene_object().transform().local_position.z -= speed * delta_time;
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
