#pragma once

#include "Audio/AudioPlatform.hpp"
#include "Font/FontLibrary.hpp"
#include "ImguiManager.hpp"
#include "Input/KeyboardDeviceSystem.hpp"
#include "Input/MouseDeviceSystem.hpp"
#include "Rendering/SceneRenderer.hpp"
#include "Resources/ResourceLoader.hpp"
#include "Resources/ResourcesModuleBackend.hpp"
#include "SceneAudio/SceneAudioSystem.hpp"
#include "SceneSerialization/SceneSerializationModuleBackend.hpp"
#include "ScreenHandler.hpp"
#include "Window.hpp"

#include <nodec_engine/impl/nodec_engine_module.hpp>
#include <nodec_input/input_devices.hpp>
#include <nodec_input/keyboard/impl/keyboard_device.hpp>
#include <nodec_input/mouse/impl/mouse_device.hpp>
#include <nodec_resources/impl/resources_module.hpp>
#include <nodec_scene/scene.hpp>
#include <nodec_scene/systems/transform_system.hpp>
#include <nodec_scene_serialization/scene_loader.hpp>
#include <nodec_scene_serialization/scene_serialization.hpp>
#include <nodec_screen/impl/screen_module.hpp>
#include <nodec_world/impl/world_module.hpp>

#include <nodec/logging.hpp>
#include <nodec/unicode.hpp>

class Engine : public nodec_engine::impl::NodecEngineModule {
    using Screen = nodec_screen::Screen;
    using ScreenModule = nodec_screen::impl::ScreenModule;

    using Scene = nodec_scene::Scene;

    using ResourcesModule = nodec_resources::impl::ResourcesModule;
    using Resources = nodec_resources::Resources;

    using SceneSerialization = nodec_scene_serialization::SceneSerialization;

public:
    Engine() {
        using namespace nodec_world;
        using namespace nodec_world::impl;

        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Created!";

        imgui_manager_.reset(new ImguiManager);

        font_library_.reset(new FontLibrary);

        // --- screen ---
        screen_module_.reset(new ScreenModule());
        add_module<Screen>(screen_module_);

        screen_handler_.reset(new ScreenHandler(screen_module_.get()));
        screen_handler_->BindHandlersOnBoot();

        // --- scene ---
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
    }

    ~Engine() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Destructed.";

        // TODO: Consider to unload all modules before backends.

        // unload all scene entities.
        world_module_->scene().registry().clear();
    }

    void setup() {
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

    void frame_begin() {
        window_->GetGraphics().BeginFrame();
    }

    void frame_end() {
        using namespace nodec::entities;
        using namespace nodec_scene::components;

        {
            auto root = world_module_->scene().hierarchy_system().root_hierarchy().first;
            while (root != null_entity) {
                nodec_scene::systems::update_transform(world_module_->scene().registry(), root);
                root = world_module_->scene().registry().get_component<Hierarchy>(root).next;
            }
        }

        scene_renderer_->Render(world_module_->scene());

        window_->GetGraphics().EndFrame();
    }

    ScreenModule &screen_module() {
        return *screen_module_;
    }

    nodec_world::impl::WorldModule &world_module() {
        return *world_module_;
    }

    ResourcesModule &resources_module() {
        return *resources_module_;
    }
    SceneSerialization &scene_serialization() {
        return *scene_serialization_module_;
    }
    AudioPlatform &audio_platform() {
        return *audio_platform_;
    }

private:
    // imgui must be destroyed after window.
    std::unique_ptr<ImguiManager> imgui_manager_;
    std::unique_ptr<Window> window_;

    std::unique_ptr<FontLibrary> font_library_;

    std::unique_ptr<AudioPlatform> audio_platform_;

    std::shared_ptr<ScreenModule> screen_module_;
    std::unique_ptr<ScreenHandler> screen_handler_;

    std::shared_ptr<nodec_input::InputDevices> input_devices_;
    KeyboardDeviceSystem *keyboard_device_system_;
    MouseDeviceSystem *mouse_device_system_;

    std::shared_ptr<nodec_scene_serialization::SceneLoader> scene_loader_;

    std::shared_ptr<ResourcesModuleBackend> resources_module_;

    std::shared_ptr<SceneSerializationModuleBackend> scene_serialization_module_;

    std::shared_ptr<nodec_world::impl::WorldModule> world_module_;

    std::unique_ptr<SceneRenderer> scene_renderer_;

    std::unique_ptr<SceneAudioSystem> scene_audio_system_;
};