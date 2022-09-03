#include "app.hpp"
#include "camera_controller.hpp"

using namespace nodec;
using namespace nodec_engine;
using namespace nodec_input;
using namespace nodec_scene;
using namespace nodec_scene::components;
using namespace nodec_screen;
using namespace nodec_resources;
using namespace nodec_rendering::components;
using namespace nodec_rendering::resources;
using namespace nodec_scene_serialization;
using namespace nodec_scene_audio::resources;
using namespace nodec_scene_audio::components;
using namespace nodec_world;

class HelloWorld {
public:
    // struct HelloComponent {
    //     int my_field;
    // };

    // class SerializableHelloComponent : public nodec_scene_serialization::BaseSerializableComponent {
    // public:
    //     SerializableHelloComponent()
    //         : BaseSerializableComponent(this) {
    //     }

    //    int my_field;

    //    template<class Archive>
    //    void serialize(Archive &archive) {
    //        archive(cereal::make_nvp("my_field", my_field));
    //    }
    //};

    HelloWorld(NodecEngine &engine)
        : engine{engine} {
        // engine.stepped().connect([=](NodecEngine &engine) { on_stepped(engine); });
        // engine.initialized().connect([=](NodecEngine &engine) { on_initialized(engine); });
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::HelloWorld] >>> Hello :)";

        auto &world = engine.get_module<World>();
        auto &input = engine.get_module<Input>();
        auto &serialization = engine.get_module<SceneSerialization>();

#ifdef EDITOR_MODE
        using namespace nodec_scene_editor;
        auto &editor = engine.get_module<SceneEditor>();

        // editor.inspector_component_registry().register_component<HelloComponent>(
        //     "Hello Component", [=](HelloComponent &comp) {
        //         /*ImGui::Text("My Field"); ImGui::SameLine();*/
        //         // ImGui::SliderInt("My Field", &comp.my_field, 0, 100);

        //        if (target_material) {
        //            float metallic, roughness;

        //            metallic = target_material->get_float_property("metallic");
        //            roughness = target_material->get_float_property("roughness");

        //            ImGui::DragFloat("metallic", &metallic, 0.01f, 0.0f, 1.0f);
        //            ImGui::DragFloat("roughness", &roughness, 0.01f, 0.0f, 1.0f);

        //            target_material->set_float_property("metallic", metallic);
        //            target_material->set_float_property("roughness", roughness);
        //        }
        //    });
#endif
        {
            world.initialized().connect([&](World &world) { on_initialized(world); });
            world.stepped().connect([&](World &world) { on_stepped(world); });
        }

        {
            camera_controller_system_ = std::make_shared<CameraControllerSystem>(world, input, serialization);
#ifdef EDITOR_MODE
            CameraControllerSystem::setup_editor(editor);
#endif
        }
        //{
        //    auto &scene_serialization = engine.get_module<SceneSerialization>();

        //    scene_serialization.register_component<HelloComponent, SerializableHelloComponent>(
        //        [](const auto &comp) {
        //            auto serializable = std::make_shared<SerializableHelloComponent>();
        //            serializable->my_field = comp.my_field;
        //            return serializable;
        //        },
        //        [](const auto &serializable, auto entity, SceneRegistry &registry) {
        //            registry.emplace_component<HelloComponent>(entity);
        //            auto &comp = registry.get_component<HelloComponent>(entity);
        //            comp.my_field = serializable.my_field;
        //        });
        //}

        //{
        //    auto &input = engine.get_module<Input>();
        //    input.keyboard().key_event().connect([=, &engine](const keyboard::KeyEvent &event) {
        //        // logging::InfoStream(__FILE__, __LINE__) << event;

        //        auto &scene = engine.get_module<Scene>();

        //        if (event.key == keyboard::Key::A && event.type == keyboard::KeyEvent::Type::Release) {
        //            logging::InfoStream(__FILE__, __LINE__) << "AAA";
        //            auto &source = scene.registry().get_component<AudioSource>(audioEntity);

        //            source.is_playing = true;
        //        }

        //        if (event.key == keyboard::Key::D && event.type == keyboard::KeyEvent::Type::Release) {
        //            logging::InfoStream(__FILE__, __LINE__) << "DDD";
        //            auto &source = scene.registry().get_component<AudioSource>(audioEntity);

        //            source.is_playing = false;
        //        }
        //    });

        //    input.mouse().mouse_event().connect([](const mouse::MouseEvent &event) {
        //        // logging::InfoStream(__FILE__, __LINE__) << event;
        //    });
        //}
    }

    ~HelloWorld() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::~HelloWorld] >>> See you ;)";
    }

private:
    void on_initialized(World &world) {
        logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::on_initialized] engine time: " << engine.engine_time();

        // auto entity = scene.create_entity("Hello World!!");
        // scene.registry().emplace_component<MeshRenderer>(entity);
        // auto& renderer = scene.registry().get_component<MeshRenderer>(entity);

        auto &resources = engine.get_module<Resources>();

        {
            auto &serialization = engine.get_module<SceneSerialization>();
            auto mainScene = resources.registry().get_resource<SerializableSceneGraph>("org.nodec.hello-nodec/scenes/main.scene").get();

            SceneEntityEmplacer{mainScene, world.scene(), entities::null_entity, serialization}.emplace_all();
        }

        //{
        //    auto entt = scene.create_entity("Camera");
        //    scene.registry().emplace_component<Camera>(entt);
        //    scene.registry().emplace_component<Light>(entt);

        //    auto &trfm = scene.registry().get_component<Transform>(entt);
        //    trfm.local_position.z = -3;
        //}

        //{
        //    target_material = resources.registry().get_resource<Material>("models/primitives/Default.material").get();

        //    auto dodon_clip = resources.registry().get_resource<AudioClip>("audios/dodon.wav").get();
        //    auto miku_clip = resources.registry().get_resource<AudioClip>("audios/miku-activated.wav").get();

        //    audioEntity = scene.create_entity("Audio Source Test");
        //    scene.registry().emplace_component<AudioSource>(audioEntity);
        //    auto &source = scene.registry().get_component<AudioSource>(audioEntity);
        //    source.clip = dodon_clip;
        //    // source.clip = miku_clip;
        //    // source.loop = true;
        //    source.is_playing = true;
        //}

        //{
        //    auto texture = resources.registry().get_resource<Texture>("textures/test.jpg ").get();
        //    auto entt = scene.create_entity("image");
        //    scene.registry().emplace_component<ImageRenderer>(entt);
        //    auto &renderer = scene.registry().get_component<ImageRenderer>(entt);

        //    renderer.image = texture;
        //    renderer.material = resources.registry().get_resource<Material>("org.nodec-rendering.essentials/materials/image-default.material").get();
        //    if (texture) {
        //        logging::InfoStream(__FILE__, __LINE__) << texture->width() << ", " << texture->height();
        //    }
        //}
    }

    void on_stepped(World &scene) {
    }

private:
    NodecEngine &engine;
    std::shared_ptr<CameraControllerSystem> camera_controller_system_;
    // std::shared_ptr<Material> target_material;
    // SceneEntity audioEntity;
};
//
// CEREAL_REGISTER_TYPE(HelloWorld::SerializableHelloComponent);
// CEREAL_REGISTER_POLYMORPHIC_RELATION(BaseSerializableComponent, HelloWorld::SerializableHelloComponent);

void nodec_engine::on_boot(NodecEngine &engine) {
    logging::InfoStream(__FILE__, __LINE__) << "[App] >>> booting...";
    logging::InfoStream(__FILE__, __LINE__) << "[App] >>> Hello world!";

    auto &screen = engine.get_module<Screen>();

     //screen.set_size({1920, 1080});
     //screen.set_resolution({1920, 1080});

    screen.set_size({1280, 720});
    screen.set_resolution({1280, 720});

    // screen.set_size({ 1368, 800 });
    // screen.set_resolution({ 1368, 800 });

    // screen.set_size({ 1600, 900 });
    // screen.set_resolution({ 1600, 900 });

    screen.set_title("[ Hello Nodec ]");

    auto &resources = engine.get_module<Resources>();

#ifdef _DEBUG
    resources.set_resource_path("C:/Users/onete/OneDrive/Documents/Projects/nodec_project/nodec/apps/hello-nodec/resources");
#endif // _DEBUG

    engine.add_module(std::make_shared<HelloWorld>(engine));
}

// void game_engine::on_boot(GameEngine& engine)
//{
//    nodec::logging::info("booting... in application layer", __FILE__, __LINE__);
//    nodec::logging::info("HELLO WORLD!", __FILE__, __LINE__);
//
//    engine.screen().set_size({ 1280, 720 });
//    engine.screen().set_resolution({ 1920, 1080 });
//    engine.screen().set_title("[ void ]");
//
//
//    auto player = NodecObject::instanciate<nodec_scene::SceneObject>("Player");
//    player->add_component<Player>();
//    App::main_camera = player->add_component<Camera>();
//    engine.root_scene_object().append_child(player);
//
//
//    auto test_object_1 = NodecObject::instanciate<nodec::nodec_scene::SceneObject>("test_1");
//    test_object_1->add_component<TestCube>();
//    test_object_1->transform().local_position.y = 1.6;
//    test_object_1->transform().local_position.z = 2;
//    //player->append_child(test_object_1);
//    engine.root_scene_object().append_child(test_object_1);
//
//
//}

//
// void game_engine::on_boot(game_engine::GameEngine& engine) {
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
//    nodec_scene::systems::append_child(engine.scene_registry(), root, child1);
//    nodec_scene::systems::append_child(engine.scene_registry(), root, child2);
//    nodec_scene::systems::append_child(engine.scene_registry(), child1, child1_1);
//
//    engine.scene_registry().emplace_component<nodec_scene::components::Name>(root);
//    engine.scene_registry().get_component<nodec_scene::components::Name>(root).name = "root";
//
//
//}
