#include "controllable.hpp"


using namespace nodec;
using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_modules::input::keyboard::interfaces;
using namespace nodec_modules::input::mouse::interfaces;
using namespace nodec_modules::screen::interfaces;

void Controllable::on_awake()
{
    enable_frame_update();
}

void Controllable::on_frame_update(Rendering& rendering)
{
    Keyboard& keyboard = get_engine()->keyboard();
    Mouse& mouse = get_engine()->mouse();
    Screen& screen = get_engine()->screen();

    float delta_time = rendering.frame_delta_time();

    float speed = 0.5f;

    //std::ostringstream oss;
    //oss << mouse.position();
    //screen.set_title(oss.str());

    if (keyboard.get_key_pressed(Key::LeftShift))
    {
        speed *= 2.0f;
    }

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
        scene_object().transform().local_position.y += speed * delta_time;
    }

    if (keyboard.get_key_pressed(Key::S))
    {
        scene_object().transform().local_position.y -= speed * delta_time;
    }

    if (keyboard.get_key_pressed(Key::J))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(0, 1, 0));
        scene_object().transform().local_rotation *= delta;
    }
    if (keyboard.get_key_pressed(Key::L))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(0, -1, 0));
        scene_object().transform().local_rotation *= delta;
    }
    if (keyboard.get_key_pressed(Key::I))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(1, 0, 0));
        scene_object().transform().local_rotation *= delta;
    }
    if (keyboard.get_key_pressed(Key::K))
    {
        Quaternionf delta;
        math::gfx::set_angle_axis(delta, delta_time * 30, Vector3f(-1, 0, 0));
        scene_object().transform().local_rotation *= delta;
    }

    if (mouse.get_button_pressed(MouseButton::Left))
    {
        scene_object().transform().local_position.z += speed * delta_time;
    }

    if (mouse.get_button_pressed(MouseButton::Right))
    {
        scene_object().transform().local_position.z -= speed * delta_time;
    }

    if (keyboard.get_key_down(Key::U))
    {
        //auto new_child = NodecObject::instanciate<scene_set::SceneObject>("child");
        //new_child->add_component<TestTriangle>();
        //new_child->add_component<Rotating>();
        //new_child->add_component<DeletedLogger>();
        //new_child->transform().local_position.z = 5.0f;
        //new_child->transform().local_position.x = -5.0f + child_ptrs.size() * 1.0f;
        //scene_object().append_child(new_child);
        //child_ptrs.push_back(new_child.get());
    }

    if (keyboard.get_key_down(Key::I))
    {
        //if (child_ptrs.size() > 0)
        //{

        //    auto child_ptr = child_ptrs.back();
        //    child_ptrs.pop_back();

        //    auto child = scene_object().remove_child(*child_ptr);
        //}

    }
}
