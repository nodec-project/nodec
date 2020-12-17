//#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/game_engine/interfaces/game_engine.hpp>
#include <nodec_modules/game_engine/interfaces/behavior.hpp>

#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/renderer.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <nodec_modules/input/keyboard/interfaces/keyboard.hpp>
#include <nodec_modules/input/keyboard/interfaces/key.hpp>

#include <nodec/scene_set/scene_object.hpp>

#include <nodec/event.hpp>
#include <nodec/logging.hpp>
#include <nodec/math/math.hpp>
#include <nodec/math/gfx.hpp>

using namespace nodec;
using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_modules::input::keyboard::interfaces;
using namespace nodec_modules::input::mouse::interfaces;
using namespace nodec_modules::screen::interfaces;

//
//class BRDFMaterial : public nodec_rendering::Material
//{
//public:
//    struct Properties
//    {
//        Vector3f diffuse = Vector3f::one;
//        float alpha = 1.0f;
//
//        Vector3f specular = Vector3f(0.5f, 0.5f, 0.5f);
//        float roughness = 0.5f;
//
//        float metalness = 0.0f;
//        float shininess = 40.0f;
//
//    } properties;
//
//    BRDFMaterial()
//        : Material("brdf")
//    {
//    }
//};

class TestTriangle : public Behavior {
public:
    using Behavior::Behavior;

    void on_awake() override {
        enable_frame_update();
    }
    void on_frame_start(Rendering& rendering) override {
        auto renderer_ = scene_object().add_component<Renderer>();
        if (auto renderer = renderer_.lock()) {
            renderer->mesh = NodecObject::instanciate<Mesh>(&rendering);

            renderer->mesh->vertices.push_back({ { 0.0f, 0.5f, 0.0f }, {0.0f, 0.0f, -1.0f} }); // front
            renderer->mesh->vertices.push_back({ { 0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, -1.0f} });
            renderer->mesh->vertices.push_back({ { -0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, -1.0f} });
            renderer->mesh->vertices.push_back({ { 0.0f, 0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} }); // back
            renderer->mesh->vertices.push_back({ { -0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });
            renderer->mesh->vertices.push_back({ { 0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });

            renderer->mesh->triangles.push_back(0);
            renderer->mesh->triangles.push_back(1);
            renderer->mesh->triangles.push_back(2);
            renderer->mesh->triangles.push_back(3);
            renderer->mesh->triangles.push_back(4);
            renderer->mesh->triangles.push_back(5);

            rendering.bind_mesh(renderer->mesh.get());
            rendering.regist_renderer(renderer);
        }
    }
    void on_frame_update(Rendering& rendering) override {

    }
};

class Rotating : public Behavior {
public:
    using Behavior::Behavior;

    void on_awake() override {
        enable_frame_update();
    }

    void on_frame_update(Rendering& rendering) override {
        float delta_time = rendering.frame_delta_time();

        Vector3f axis(0, 1, 0);
        float angle = get_engine()->engine_time() * 20.0f;
        math::gfx::set_angle_axis(scene_object().transform().local_rotation, angle, axis);

    }
};

class Movable : public Behavior {
public:
    using Behavior::Behavior;

    void on_awake() override {
        enable_frame_update();
    }

    void on_frame_update(Rendering& rendering) override {
        Keyboard& keyboard = get_engine()->keyboard();
        Mouse& mouse = get_engine()->mouse();
        Screen& screen = get_engine()->screen();
        
        float delta_time = rendering.frame_delta_time();

        float speed = 0.5f;

        std::ostringstream oss;
        oss << mouse.position();
        screen.set_title(oss.str());

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


        if (mouse.get_button_pressed(MouseButton::Left))
        {
            scene_object().transform().local_position.z += speed * delta_time;
        }

        if (mouse.get_button_pressed(MouseButton::Right))
        {
            scene_object().transform().local_position.z -= speed * delta_time;
        }
    }
};

class TestCube : public Behavior
{
public:
    using Behavior::Behavior;

    void on_awake() override
    {
        //logging::DebugStream(__FILE__, __LINE__) << "Hello. I am " << name << ". I wake up." << std::flush;
        enable_frame_update();
        //enable_frame_update();
    }
    void on_frame_start(Rendering& rendering) override
    {
        //logging::DebugStream(__FILE__, __LINE__) << "start" << std::flush;
        auto renderer_ref = scene_object().add_component<Renderer>();
        if (auto renderer = renderer_ref.lock())
        {
            //renderer->mesh = NodecObject::instanciate<Mesh>(&rendering);

            //renderer->mesh->vertices.push_back({ -0.5, -0.5, -0.5 });
            //renderer->mesh->vertices.push_back({ -0.5, -0.5, +0.5 });
            //renderer->mesh->vertices.push_back({ +0.5, -0.5, -0.5 });
            //renderer->mesh->vertices.push_back({ +0.5, -0.5, +0.5 });
            //renderer->mesh->vertices.push_back({ -0.5, +0.5, -0.5 });
            //renderer->mesh->vertices.push_back({ -0.5, +0.5, +0.5 });
            //renderer->mesh->vertices.push_back({ +0.5, +0.5, -0.5 });
            //renderer->mesh->vertices.push_back({ +0.5, +0.5, +0.5 });

            //renderer->mesh->triangles.push_back(0);
            //renderer->mesh->triangles.push_back(2);
            //renderer->mesh->triangles.push_back(1);

            //renderer->mesh->triangles.push_back(1);
            //renderer->mesh->triangles.push_back(2);
            //renderer->mesh->triangles.push_back(3);

            //renderer->mesh->triangles.push_back(2);
            //renderer->mesh->triangles.push_back(6);
            //renderer->mesh->triangles.push_back(3);

            //renderer->mesh->triangles.push_back(6);
            //renderer->mesh->triangles.push_back(7);
            //renderer->mesh->triangles.push_back(3);

            //renderer->mesh->triangles.push_back(7);
            //renderer->mesh->triangles.push_back(5);
            //renderer->mesh->triangles.push_back(1);

            //renderer->mesh->triangles.push_back(7);
            //renderer->mesh->triangles.push_back(1);
            //renderer->mesh->triangles.push_back(3);

            //renderer->mesh->triangles.push_back(5);
            //renderer->mesh->triangles.push_back(4);
            //renderer->mesh->triangles.push_back(1);

            //renderer->mesh->triangles.push_back(1);
            //renderer->mesh->triangles.push_back(4);
            //renderer->mesh->triangles.push_back(0);

            //renderer->mesh->triangles.push_back(4);
            //renderer->mesh->triangles.push_back(6);
            //renderer->mesh->triangles.push_back(0);

            //renderer->mesh->triangles.push_back(6);
            //renderer->mesh->triangles.push_back(2);
            //renderer->mesh->triangles.push_back(0);

            //renderer->mesh->triangles.push_back(5);
            //renderer->mesh->triangles.push_back(7);
            //renderer->mesh->triangles.push_back(6);

            //renderer->mesh->triangles.push_back(4);
            //renderer->mesh->triangles.push_back(5);
            //renderer->mesh->triangles.push_back(6);

            //throw nodec::NodecException("TEST", __FILE__, __LINE__);
            //renderer->mesh->bind(&(nodec_game_engine::get_engine()->rendering()));
            //rendering.bind_mesh(renderer->mesh.get());
            //rendering.regist_renderer(renderer);
        }
    }
    void on_frame_update(Rendering& rendering) override
    {


    }
};


//void on_frame_update(Rendering& rendering)
//{
//    //nodec::logging::debug("frame update", __FILE__, __LINE__);
//    //nodec::logging::DebugStream(__FILE__, __LINE__) << rendering.frame_delta_time() << std::flush;
//    logging::debug("1", __FILE__, __LINE__);
//}
//
//void on_frame_update_2(Rendering& rendering)
//{
//    logging::debug("2", __FILE__, __LINE__);
//}

void nodec_modules::game_engine::interfaces::on_boot(GameEngine& engine)
{
    nodec::logging::info("booting... in application layer", __FILE__, __LINE__);
    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);

    engine.screen().set_size({ 1280, 720 });
    engine.screen().set_resolution({ 1920, 1080 });
    engine.screen().set_title("[ void ]");

    //auto on_frame_update_callback = nodec::event::StaticCallback<nodec_rendering::Rendering&>::make_shared(&on_frame_update);
    //auto on_frame_update_2_callback = nodec::event::StaticCallback<nodec_rendering::Rendering&>::make_shared(&on_frame_update_2);
    //engine.rendering().on_frame_update += on_frame_update_callback;
    //engine.rendering().on_frame_update += on_frame_update_2_callback;


    //auto test_object_1 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_object_1");
    //engine.root_scene_object().append_child(test_object_1);


    //auto test_object_2 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_object_2");
    //test_object_1->append_child(test_object_2);

    ////auto testtest_object = NodecObject::instanciate<nodec::scene_set::SceneObject>("testtest_object");
    ////testtest_object->append_child(test_object); // ERROR

    //test_object_1->add_component<TestCube>();
    //test_object_1->transform().local_position.x = 0.0f;
    //test_object_1->transform().local_position.z = 3.0f;

    //test_object_2->add_component<TestCube>();
    //test_object_2->transform().local_position.x = 0.0f;
    //test_object_2->transform().local_position.z = 3.0f;

    auto test_triangle_1 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_triangle_1");
    engine.root_scene_object().append_child(test_triangle_1);
    test_triangle_1->add_component<TestTriangle>();
    test_triangle_1->add_component<Movable>();
    test_triangle_1->transform().local_position.z = 3.0f;


    auto test_triangle_2 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_triangle_2");
    engine.root_scene_object().append_child(test_triangle_2);
    test_triangle_2->add_component<TestTriangle>();
    test_triangle_2->add_component<Rotating>();
    test_triangle_2->transform().local_position.z = 5.0f;
}
