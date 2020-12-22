#include "app.hpp"

#include "test_cube.hpp"
#include "controllable.hpp"

using namespace nodec;
using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_modules::input::keyboard::interfaces;
using namespace nodec_modules::input::mouse::interfaces;
using namespace nodec_modules::screen::interfaces;
using namespace nodec_extentions::material_set;

//
//
//class DeletedLogger : public Behavior
//{
//public:
//    using Behavior::Behavior;
//
//    void on_awake() override
//    {
//    }
//
//    ~DeletedLogger()
//    {
//        logging::DebugStream(__FILE__, __LINE__) << "I '" << scene_object().name << "' was deleted.";
//        //logging::DebugStream(__FILE__, __LINE__) << "is it ok? " << get_engine()->engine_time();
//    }
//};
//
//class TestTriangle : public Behavior {
//public:
//    static NodecObject::Reference<Mesh> mesh_global;
//
//public:
//    using Behavior::Behavior;
//
//private:
//    NodecObject::Reference<Renderer> renderer_;
//
//public:
//    void on_awake() override {
//        enable_frame_update();
//    }
//    void on_frame_start(Rendering& rendering) override {
//        renderer_ = scene_object().add_component<Renderer>();
//
//        if (auto renderer = renderer_.lock()) {
//
//            if (auto mesh = mesh_global.lock())
//            {
//                renderer->mesh = mesh;
//
//                logging::debug("reused", __FILE__, __LINE__);
//            }
//            else
//            {
//                auto new_mesh = NodecObject::instanciate<Mesh>(&rendering);
//
//                new_mesh->vertices.push_back({ { 0.0f, 0.5f, 0.0f }, {0.0f, 0.0f, -1.0f} }); // front
//                new_mesh->vertices.push_back({ { 0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, -1.0f} });
//                new_mesh->vertices.push_back({ { -0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, -1.0f} });
//                new_mesh->vertices.push_back({ { 0.0f, 0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} }); // back
//                new_mesh->vertices.push_back({ { -0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });
//                new_mesh->vertices.push_back({ { 0.5f, -0.5f, 0.0f }, {0.0f, 0.0f, 1.0f} });
//                
//                new_mesh->triangles.push_back(0);
//                new_mesh->triangles.push_back(1);
//                new_mesh->triangles.push_back(2);
//                new_mesh->triangles.push_back(3);
//                new_mesh->triangles.push_back(4);
//                new_mesh->triangles.push_back(5);
//
//                rendering.bind_mesh(new_mesh.get());
//
//                renderer->mesh = new_mesh;
//                mesh_global = new_mesh;
//
//                logging::debug("created", __FILE__, __LINE__);
//            }
//
//            auto material = NodecObject::instanciate<BRDFMaterial>(&rendering);
//            rendering.bind_material(material.get());
//            renderer->material = material;
//
//            renderer->material->set_vector4("albedo", Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
//
//            rendering.regist_renderer(renderer);
//
//            renderer_ = renderer;
//        }
//    }
//    void on_frame_update(Rendering& rendering) override {
//
//        Screen& screen = get_engine()->screen();
//
//        if (auto renderer = renderer_.lock())
//        {
//            //auto engine_time = get_engine()->engine_time();
//
//            //auto metalness = (1 + std::sinf(engine_time))/2;
//            //auto roughness = (1 + std::cosf(engine_time))/2;
//            //renderer->material->set_float("metalness", metalness);
//            //renderer->material->set_float("roughness", roughness);
//
//            //std::ostringstream oss;
//            //oss << metalness << ", " << roughness;
//
//            //nodec::logging::DebugStream(__FILE__, __LINE__) << oss.str();
//            //screen.set_title(oss.str());
//        }
//    }
//};
//
//NodecObject::Reference<Mesh> TestTriangle::mesh_global;
//
//class Rotating : public Behavior {
//public:
//    using Behavior::Behavior;
//
//    void on_awake() override {
//        enable_frame_update();
//    }
//
//    void on_frame_update(Rendering& rendering) override {
//        float delta_time = rendering.frame_delta_time();
//
//        Vector3f axis(0, 1, 0);
//        float angle = get_engine()->engine_time() * 20.0f;
//        math::gfx::set_angle_axis(scene_object().transform().local_rotation, angle, axis);
//
//    }
//};
//


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

    auto test_object_1 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_1");
    engine.root_scene_object().append_child(test_object_1);
    test_object_1->add_component<TestCube>();
    test_object_1->add_component<Controllable>();
    test_object_1->transform().local_position.z = 3.0f;


    //auto test_triangle_2 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_2");
    //engine.root_scene_object().append_child(test_triangle_2);
    ////test_triangle_2->add_component<TestTriangle>();
    //test_triangle_2->add_component<Rotating>();
    //test_triangle_2->transform().local_position.z = 5.0f;
}
