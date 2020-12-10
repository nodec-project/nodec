//#include <nodec_modules/input/keyboard.hpp>
#include <nodec_modules/game_engine/interfaces/game_engine.hpp>

#include <nodec_modules/rendering/interfaces/material.hpp>
#include <nodec_modules/rendering/interfaces/mesh.hpp>
#include <nodec_modules/rendering/interfaces/renderer.hpp>
#include <nodec_modules/rendering/interfaces/rendering.hpp>

#include <nodec/scene_set/scene_object.hpp>
#include <nodec/scene_set/behavior.hpp>

#include <nodec/event.hpp>
#include <nodec/logging.hpp>

using namespace nodec;
namespace nodec_game_engine = nodec_modules::game_engine::interfaces;
namespace nodec_rendering = nodec_modules::rendering::interfaces;

nodec_game_engine::GameEngine* p_engine;

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

void on_frame_update(nodec_rendering::Rendering& rendering)
{
    //nodec::logging::debug("frame update", __FILE__, __LINE__);
    //nodec::logging::DebugStream(__FILE__, __LINE__) << rendering.frame_delta_time() << std::flush;
}

void on_boot(nodec_game_engine::GameEngine& engine)
{
    nodec::logging::info("booting...", __FILE__, __LINE__);
    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);

    auto on_frame_update_callback = nodec::event::StaticCallback<nodec_rendering::Rendering&>::make_shared(&on_frame_update);
    engine.rendering().on_frame_update += on_frame_update_callback;

    auto test_object = nodec::NodecObject::instanciate<nodec::scene_set::SceneObject>("test_object");

    nodec::logging::DebugStream(__FILE__, __LINE__) << test_object->name << std::flush;

    auto renderer_ref = test_object->add_component<nodec_rendering::Renderer>();
    if (auto renderer = renderer_ref.lock())
    {
        renderer->mesh = nodec::NodecObject::instanciate<nodec_rendering::Mesh>();
        renderer->mesh->vertices.push_back({ -0.5, -0.5, -0.5 });
        renderer->mesh->vertices.push_back({ +0.5, -0.5, -0.5 });
        renderer->mesh->vertices.push_back({ +0.5, -0.5, +0.5 });
        renderer->mesh->vertices.push_back({ -0.5, -0.5, +0.5 });
        renderer->mesh->vertices.push_back({ -0.5, +0.5, -0.5 });
        renderer->mesh->vertices.push_back({ +0.5, +0.5, -0.5 });
        renderer->mesh->vertices.push_back({ +0.5, +0.5, +0.5 });
        renderer->mesh->vertices.push_back({ -0.5, +0.5, +0.5 });

        //throw nodec::NodecException("TEST", __FILE__, __LINE__);
        renderer->mesh->bind(&engine.rendering());
    }

    p_engine = &engine;
}
