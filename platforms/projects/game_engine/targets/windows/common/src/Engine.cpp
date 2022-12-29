#include <Engine.hpp>

Engine::Engine() {
    using namespace nodec_world;
    using namespace nodec_world::impl;
    using namespace nodec_screen::impl;
    using namespace nodec_screen;

    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Created!";

    imgui_manager_.reset(new ImguiManager);

    font_library_.reset(new FontLibrary);

    // --- screen ---
    screen_module_.reset(new ScreenModule());
    add_module<Screen>(screen_module_);

    screen_handler_.reset(new ScreenHandler(screen_module_.get()));
    screen_handler_->BindHandlersOnBoot();

    // --- world ---
    world_module_.reset(new WorldModule());
    add_module<World>(world_module_);

    // --- input ---
    input_devices_.reset(new nodec_input::InputDevices());
    add_module<nodec_input::InputDevices>(input_devices_);

    keyboard_device_system_ = &input_devices_->emplace_device_system<KeyboardDeviceSystem>();
    mouse_device_system_ = &input_devices_->emplace_device_system<MouseDeviceSystem>();

    // --- resources ---
    resources_module_.reset(new ResourcesModuleBackend());
    add_module<Resources>(resources_module_);

    resources_module_->setup_on_boot();

    // --- scene serialization ---
    scene_serialization_module_.reset(new SceneSerializationModuleBackend(&resources_module_->registry()));
    add_module<SceneSerialization>(scene_serialization_module_);

    scene_loader_.reset(new nodec_scene_serialization::SceneLoader(*scene_serialization_module_, world_module_->scene(), resources_module_->registry()));
    add_module<nodec_scene_serialization::SceneLoader>(scene_loader_);

    // ---

    physics_system_.reset(new PhysicsSystemBackend(*world_module_));

    visibility_system_.reset(new nodec_rendering::systems::VisibilitySystem(world_module_->scene()));
}

void Engine::setup() {
    using namespace nodec;

    window_.reset(new Window(
        screen_module_->internal_size.x, screen_module_->internal_size.y,
        screen_module_->internal_resolution.x, screen_module_->internal_resolution.y,
        unicode::utf8to16<std::wstring>(screen_module_->internal_title).c_str(),
        &keyboard_device_system_->device(), &mouse_device_system_->device()));

    screen_handler_->Setup(window_.get());

    resources_module_->setup_on_runtime(&window_->GetGraphics(), font_library_.get());

    scene_renderer_.reset(new SceneRenderer(&window_->GetGraphics(), resources_module_->registry()));

    audio_platform_.reset(new AudioPlatform());

    scene_audio_system_.reset(new SceneAudioSystem(audio_platform_.get(), &world_module_->scene().registry()));

    world_module_->stepped().connect([&](auto &world) {
        scene_audio_system_->UpdateAudio(world_module_->scene().registry());
    });
}

void Engine::frame_end() {
    using namespace nodec::entities;
    using namespace nodec_scene::components;

    {
        auto root = world_module_->scene().hierarchy_system().root_hierarchy().first;
        while (root != null_entity) {
            nodec_scene::systems::update_transform(world_module_->scene().registry(), root);
            root = world_module_->scene().registry().get_component<Hierarchy>(root).next;
        }
    }

    scene_renderer_->Render(world_module_->scene(),
                            &window_->GetGraphics().GetRenderTargetView(),
                            window_->GetGraphics().GetWidth(), window_->GetGraphics().GetHeight());

    window_->GetGraphics().EndFrame();
}
