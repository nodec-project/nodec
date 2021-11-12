#include "app.hpp"
//
//#include "test_cube.hpp"
//#include "player.hpp"

using namespace nodec;
using namespace nodec_engine;
using namespace scene_set;
using namespace screen;
using namespace resources;
using namespace rendering::resources;


class HelloWorld {

public:
    struct HelloComponent {
        int my_field;
    };

    HelloWorld(NodecEngine& engine) {
        engine.stepped().connect([=](NodecEngine& engine) { on_stepped(engine); });
        engine.initialized().connect([=](NodecEngine& engine) { on_initialized(engine); });
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::HelloWorld] >>> Hello :)";

#ifdef EDITOR_MODE
        using namespace scene_editor;
        auto& editor = engine.get_module<SceneEditor>();

        editor.inspector_component_registry().register_component<HelloComponent>("HelloComponent", [](HelloComponent& comp) {
            /*ImGui::Text("My Field"); ImGui::SameLine();*/
            ImGui::SliderInt("My Field", &comp.my_field, 0, 100);
                                                                                 });
#endif
    }

    ~HelloWorld() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::~HelloWorld] >>> See you ;)";
    }



private:

    void on_initialized(NodecEngine& engine) {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::on_initialized] engine time: " << engine.engine_time();

        auto& scene = engine.get_module<Scene>();
        auto entity = scene.create_entity("Hello World!!");

        auto& resources = engine.get_module<Resources>();
        mesh_future = resources.registry().get_resource<Mesh>("models/retrotv/Circle.001##mesh-2.mesh");
    }

    void on_stepped(NodecEngine& engine) {
        logging::InfoStream info(__FILE__, __LINE__);

        if (mesh_future.valid()) {
            auto result = mesh_future.wait_for(std::chrono::nanoseconds(1));
            if (result != std::future_status::timeout) {
                info << mesh_future.get();
                mesh_future = {}; // reset
            }
            else {
                info << "timeout";
            }
        }

    }

private:
    
    std::shared_future<std::shared_ptr<Mesh>> mesh_future;
};

void nodec_engine::on_boot(NodecEngine& engine) {
    using namespace nodec;
    logging::InfoStream(__FILE__, __LINE__) << "[App] >>> booting...";
    logging::InfoStream(__FILE__, __LINE__) << "[App] >>> Hello world!";

    auto& screen = engine.get_module<Screen>();

    screen.set_size({ 1920, 1080 });
    //screen.set_resolution({ 1920, 1080 });

    //screen.set_size({ 1280, 720 });
    screen.set_resolution({ 1280, 720 });

    //screen.set_size({ 1368, 800 });
    //screen.set_resolution({ 1368, 800 });

    //screen.set_size({ 1600, 900 });
    //screen.set_resolution({ 1600, 900 });

    screen.set_title("[ void ]");

    auto& resources = engine.get_module<Resources>();

#ifdef _DEBUG
    resources.set_resource_path("C:/Users/onete/OneDrive/Documents/Projects/nodec_project/nodec/apps/void/resources");
#endif // _DEBUG

    engine.add_module(std::make_shared<HelloWorld>(engine));

}

//
//using namespace nodec;
//using namespace game_engine;
//using namespace nodec_modules::rendering::interfaces;
//using namespace nodec_modules::screen::interfaces;
//using namespace nodec_extentions::material_set;
//
//NodecObject::Reference<Camera> App::main_camera;
//
//
//void game_engine::on_boot(GameEngine& engine)
//{
//    nodec::logging::info("booting... in application layer", __FILE__, __LINE__);
//    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);
//
//    engine.screen().set_size({ 1280, 720 });
//    engine.screen().set_resolution({ 1920, 1080 });
//    engine.screen().set_title("[ void ]");
//
//
//    auto player = NodecObject::instanciate<scene_set::SceneObject>("Player");
//    player->add_component<Player>();
//    App::main_camera = player->add_component<Camera>();
//    engine.root_scene_object().append_child(player);
//
//    
//    auto test_object_1 = NodecObject::instanciate<nodec::scene_set::SceneObject>("test_1");
//    test_object_1->add_component<TestCube>();
//    test_object_1->transform().local_position.y = 1.6;
//    test_object_1->transform().local_position.z = 2;
//    //player->append_child(test_object_1);
//    engine.root_scene_object().append_child(test_object_1);
//
//
//}

//
//void game_engine::on_boot(game_engine::GameEngine& engine) {
//    logging::info("booting... in application layer", __FILE__, __LINE__);
//    logging::info("HELLO WORLD!", __FILE__, __LINE__);
//
//    
//
//    //engine.screen().set_size({ 1920, 1080 });
//    engine.screen().set_size({ 1280, 720 });
//    engine.screen().set_resolution({ 1280, 720 });
//    engine.screen().set_title("[ void ]");
//
//    auto root = engine.scene_registry().create_entity();
//    auto child1 = engine.scene_registry().create_entity();
//    auto child1_1 = engine.scene_registry().create_entity();
//    auto child2 = engine.scene_registry().create_entity();
//
//    scene_set::systems::append_child(engine.scene_registry(), root, child1);
//    scene_set::systems::append_child(engine.scene_registry(), root, child2);
//    scene_set::systems::append_child(engine.scene_registry(), child1, child1_1);
//
//    engine.scene_registry().emplace_component<scene_set::components::Name>(root);
//    engine.scene_registry().get_component<scene_set::components::Name>(root).name = "root";
//    
//    
//}

