//#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/game_engine/interfaces/game_engine.hpp>
#include <nodec_modules/game_engine/interfaces/behavior.hpp>

#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/renderer.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <nodec_modules/input/interfaces/keyboard.hpp>
#include <nodec_modules/input/interfaces/key.hpp>

#include <nodec/scene_set/scene_object.hpp>

#include <nodec/event.hpp>
#include <nodec/logging.hpp>
#include <nodec/gmath.hpp>

using namespace nodec;
namespace nodec_game_engine = nodec_modules::game_engine::interfaces;
namespace nodec_rendering = nodec_modules::rendering::interfaces;

using namespace nodec_modules::input::interfaces;

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

class TestCube : public nodec_game_engine::Behavior
{
public:
    using Behavior::Behavior;

    void on_awake() override
    {
        logging::DebugStream(__FILE__, __LINE__) << "Hello. I am " << name << ". I wake up." << std::flush;



        enable_frame_update();
        //enable_frame_update();
    }
    void on_frame_start(nodec_rendering::Rendering& rendering) override
    {
        logging::DebugStream(__FILE__, __LINE__) << "start" << std::flush;
        auto renderer_ref = scene_object().add_component<nodec_rendering::Renderer>();
        if (auto renderer = renderer_ref.lock())
        {
            renderer->mesh = NodecObject::instanciate<nodec_rendering::Mesh>(&rendering);

            renderer->mesh->vertices.push_back({ { 0.0f, 0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });
            renderer->mesh->vertices.push_back({ { 0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });
            renderer->mesh->vertices.push_back({ { -0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });



            renderer->mesh->triangles.push_back(0);
            renderer->mesh->triangles.push_back(1);
            renderer->mesh->triangles.push_back(2);


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
            rendering.bind_mesh(renderer->mesh.get());
            rendering.regist_renderer(renderer);
        }
    }
    void on_frame_update(nodec_rendering::Rendering& rendering) override
    {
        Keyboard& keyboard = nodec_game_engine::get_engine()->keyboard();
        float delta_time = rendering.frame_delta_time();
        float speed = 0.5f;

        Vector3f axis(0, 1, 0);
        float angle = nodec_game_engine::get_engine()->engine_time() * 10.0f;
        gmath::set_angle_axis(scene_object().transform().local_rotation, angle, axis);
        

        if (keyboard.get_key_pressed(Key::LeftShift))
        {
            logging::debug("dad", __FILE__, __LINE__);
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

        



        //logging::DebugStream(__FILE__, __LINE__) << "update" << std::flush;
        //throw NodecException("TEST", __FILE__, __LINE__);
        //logging::DebugStream(__FILE__, __LINE__) << rendering.frame_delta_time();
    }
};


void on_frame_update(nodec_rendering::Rendering& rendering)
{
    //nodec::logging::debug("frame update", __FILE__, __LINE__);
    //nodec::logging::DebugStream(__FILE__, __LINE__) << rendering.frame_delta_time() << std::flush;
    logging::debug("1", __FILE__, __LINE__);
}

void on_frame_update_2(nodec_rendering::Rendering& rendering)
{

    logging::debug("2", __FILE__, __LINE__);
}

void nodec_game_engine::on_boot(nodec_game_engine::GameEngine& engine)
{
    nodec::logging::info("booting... in application layer", __FILE__, __LINE__);
    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);

    //auto on_frame_update_callback = nodec::event::StaticCallback<nodec_rendering::Rendering&>::make_shared(&on_frame_update);
    //auto on_frame_update_2_callback = nodec::event::StaticCallback<nodec_rendering::Rendering&>::make_shared(&on_frame_update_2);
    //engine.rendering().on_frame_update += on_frame_update_callback;
    //engine.rendering().on_frame_update += on_frame_update_2_callback;


    auto test_object_1 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_object_1");
    engine.root_scene_object().append_child(test_object_1);


    auto test_object_2 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_object_2");
    test_object_1->append_child(test_object_2);

    //auto testtest_object = NodecObject::instanciate<nodec::scene_set::SceneObject>("testtest_object");
    //testtest_object->append_child(test_object); // ERROR

    //nodec::logging::DebugStream(__FILE__, __LINE__) << test_object_1->name << std::flush;
    test_object_1->add_component<TestCube>();
    test_object_1->transform().local_position.x = 0.0f;
    test_object_1->transform().local_position.z = 3.0f;

    test_object_2->add_component<TestCube>();
    test_object_2->transform().local_position.x = 0.0f;
    test_object_2->transform().local_position.z = 3.0f;

    logging::InfoStream(__FILE__, __LINE__) << nodec_game_engine::get_engine()->id() << std::flush;
    logging::InfoStream(__FILE__, __LINE__) << nodec_game_engine::get_engine()->name << std::flush;
    logging::InfoStream(__FILE__, __LINE__) << nodec_game_engine::get_engine()->engine_time() << std::flush;
}
