#include "app.hpp"

#include "test_cube.hpp"
#include "player.hpp"

using namespace nodec;
using namespace nodec_modules::game_engine::interfaces;
using namespace nodec_modules::rendering::interfaces;
using namespace nodec_modules::input::keyboard::interfaces;
using namespace nodec_modules::input::mouse::interfaces;
using namespace nodec_modules::screen::interfaces;
using namespace nodec_extentions::material_set;

NodecObject::Reference<Camera> App::main_camera;


void nodec_modules::game_engine::interfaces::on_boot(GameEngine& engine)
{
    nodec::logging::info("booting... in application layer", __FILE__, __LINE__);
    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);

    engine.screen().set_size({ 1280, 720 });
    engine.screen().set_resolution({ 1920, 1080 });
    engine.screen().set_title("[ void ]");


    auto player = NodecObject::instanciate<scene_set::SceneObject>("Player");
    player->add_component<Player>();
    App::main_camera = player->add_component<Camera>();
    engine.root_scene_object().append_child(player);

    
    auto test_object_1 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_1");
    test_object_1->add_component<TestCube>();
    player->append_child(test_object_1);


}
