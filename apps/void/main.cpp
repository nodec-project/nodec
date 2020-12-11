//#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/game_engine/interfaces/game_engine.hpp>
#include <nodec_modules/game_engine/interfaces/behavior.hpp>

#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/renderer.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <nodec/scene_set/scene_object.hpp>

#include <nodec/event.hpp>
#include <nodec/logging.hpp>

using namespace nodec;
namespace nodec_game_engine = nodec_modules::game_engine::interfaces;
namespace nodec_rendering = nodec_modules::rendering::interfaces;


class BRDFMaterial : public nodec_rendering::Material
{
public:
    struct Properties
    {
        Vector3f diffuse = Vector3f::one;
        float alpha = 1.0f;

        Vector3f specular = Vector3f(0.5f, 0.5f, 0.5f);
        float roughness = 0.5f;

        float metalness = 0.0f;
        float shininess = 40.0f;

    } properties;

    BRDFMaterial()
        : Material("brdf")
    {
    }
};

class TestBehavior : public nodec_game_engine::Behavior
{
public:
    using Behavior::Behavior;

    void on_awake() override
    {
        logging::DebugStream(__FILE__, __LINE__) << "Hello. I am " << name <<". I wake up." << std::flush;

        auto renderer_ref = scene_object().add_component<nodec_rendering::Renderer>();
        if (auto renderer = renderer_ref.lock())
        {
            renderer->mesh = NodecObject::instanciate<nodec_rendering::Mesh>();
            renderer->mesh->vertices.push_back({ -0.5, -0.5, -0.5 });
            renderer->mesh->vertices.push_back({ +0.5, -0.5, -0.5 });
            renderer->mesh->vertices.push_back({ +0.5, -0.5, +0.5 });
            renderer->mesh->vertices.push_back({ -0.5, -0.5, +0.5 });
            renderer->mesh->vertices.push_back({ -0.5, +0.5, -0.5 });
            renderer->mesh->vertices.push_back({ +0.5, +0.5, -0.5 });
            renderer->mesh->vertices.push_back({ +0.5, +0.5, +0.5 });
            renderer->mesh->vertices.push_back({ -0.5, +0.5, +0.5 });

            //throw nodec::NodecException("TEST", __FILE__, __LINE__);
            renderer->mesh->bind(&(nodec_game_engine::get_engine()->rendering()));
        }

        enable_frame_update();
        //enable_frame_update();
    }
    void on_frame_start(nodec_rendering::Rendering& rendering) override
    {
        logging::DebugStream(__FILE__, __LINE__) << "start" << std::flush;

    }
    void on_frame_update(nodec_rendering::Rendering& rendering) override
    {
        logging::DebugStream(__FILE__, __LINE__) << "update" << std::flush;
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

    auto test_object = nodec::NodecObject::instanciate<nodec::scene_set::SceneObject>("test_object");
    auto testtest_object = nodec::NodecObject::instanciate<nodec::scene_set::SceneObject>("testtest_object");
    engine.root_scene_object().append_child(test_object);
    //testtest_object->append_child(test_object); // ERROR

    nodec::logging::DebugStream(__FILE__, __LINE__) << test_object->name << std::flush;
    test_object->add_component<TestBehavior>();
    
    logging::InfoStream(__FILE__, __LINE__) << nodec_game_engine::get_engine()->id() << std::flush;
    logging::InfoStream(__FILE__, __LINE__) << nodec_game_engine::get_engine()->name << std::flush;
    logging::InfoStream(__FILE__, __LINE__) << nodec_game_engine::get_engine()->engine_time() << std::flush;
}
