#include "app.hpp"
#include "camera_controller.hpp"
#include "light_particle.hpp"
#include "object_spawn_system.hpp"
#include "scene_transition_system.hpp"

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
using namespace nodec_input::keyboard;
using namespace nodec_input::mouse;

class HelloWorld {
public:
    HelloWorld(NodecEngine &engine)
        : engine{engine} {
        // engine.stepped().connect([=](NodecEngine &engine) { on_stepped(engine); });
        // engine.initialized().connect([=](NodecEngine &engine) { on_initialized(engine); });
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::HelloWorld] >>> Hello :)";

        auto &world = engine.get_module<World>();
        auto &input_devices = engine.get_module<InputDevices>();
        auto &serialization = engine.get_module<SceneSerialization>();
        auto &resources = engine.get_module<Resources>();
        auto &scene_loader = engine.get_module<SceneLoader>();

        auto keyboard = input_devices.get_available_devices<Keyboard>().front();
        auto mouse = input_devices.get_available_devices<Mouse>().front();

#ifdef EDITOR_MODE
        using namespace nodec_scene_editor;
        auto &editor = engine.get_module<SceneEditor>();

#endif
        {
            world.initialized().connect([&](World &world) { on_initialized(world); });
            world.stepped().connect([&](World &world) { on_stepped(world); });
        }

        {
            camera_controller_system_ = std::make_shared<CameraControllerSystem>(world, keyboard, mouse, serialization);
#ifdef EDITOR_MODE
            CameraControllerSystem::setup_editor(editor);
#endif
        }
        {
            light_particle = std::make_shared<LightParticle>(world, resources.registry(), serialization);
        }

        {
            object_spawn_system_ = std::make_shared<ObjectSpawnSystem>(keyboard, world, serialization, scene_loader);
#ifdef EDITOR_MODE
            ObjectSpawnSystem::setup_editor(editor);
#endif
        }

        {
            scene_transition_system_ = std::make_shared<SceneTransitionSystem>(world, serialization, scene_loader);
#ifdef EDITOR_MODE
            SceneTransitionSystem::setup_editor(editor);
#endif
        }

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
    }

    ~HelloWorld() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::~HelloWorld] >>> See you ;)";
    }

private:
    void on_initialized(World &world) {
        logging::InfoStream(__FILE__, __LINE__) << "[HelloWorld::on_initialized] engine time: " << engine.engine_time();

        auto &resources = engine.get_module<Resources>();

        {
            auto &serialization = engine.get_module<SceneSerialization>();
            auto mainScene = resources.registry().get_resource<SerializableSceneGraph>("org.nodec.hello-nodec/scenes/main.scene").get();

            SceneEntityEmplacer{mainScene, world.scene(), entities::null_entity, serialization}.emplace_all();
        }

        //{
        //    using namespace nodec_physics::components;

        //    auto entt = world.scene().create_entity();

        //    auto &rb = world.scene().registry().emplace_component<RigidBody>(entt).first;

        //    rb.mass = 10;

        //    world.scene().registry().emplace_component<PhysicsShape>(entt);

        //    temp = entt;
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
    }

    void on_stepped(World &world) {
        //world.scene().registry().remove_component<nodec_physics::components::PhysicsShape>(temp);
    }

private:
    //SceneEntity temp;

    NodecEngine &engine;
    std::shared_ptr<CameraControllerSystem> camera_controller_system_;
    std::shared_ptr<LightParticle> light_particle;
    std::shared_ptr<ObjectSpawnSystem> object_spawn_system_;
    std::shared_ptr<SceneTransitionSystem> scene_transition_system_;
};

void nodec_engine::on_boot(NodecEngine &engine) {
    logging::InfoStream(__FILE__, __LINE__) << "[App] >>> booting...";
    logging::InfoStream(__FILE__, __LINE__) << "[App] >>> Hello world!";

    auto &screen = engine.get_module<Screen>();

    // screen.set_size({1920, 1080});
    // screen.set_resolution({1920, 1080});

    screen.set_size({1280, 720});
    screen.set_resolution({1280, 720});

    // screen.set_size({ 1368, 800 });
    // screen.set_resolution({ 1368, 800 });

    // screen.set_size({ 1600, 900 });
    // screen.set_resolution({ 1600, 900 });

    screen.set_title("[ Hello Nodec ]");

    auto &resources = engine.get_module<Resources>();

    //#ifdef _DEBUG
    // resources.set_resource_path("C:/Users/onete/OneDrive/Documents/Projects/nodec_project/nodec/apps/hello-nodec/resources");
    //#endif // _DEBUG

    engine.add_module(std::make_shared<HelloWorld>(engine));
}
