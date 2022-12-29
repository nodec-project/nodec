#pragma once

#include "Audio/AudioPlatform.hpp"
#include "Font/FontLibrary.hpp"
#include "ImguiManager.hpp"
#include "Input/KeyboardDeviceSystem.hpp"
#include "Input/MouseDeviceSystem.hpp"
#include "Physics/PhysicsSystemBackend.hpp"
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
#include <nodec_rendering/systems/visibility_system.hpp>
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

    using Scene = nodec_scene::Scene;

    using ResourcesModule = nodec_resources::impl::ResourcesModule;
    using Resources = nodec_resources::Resources;

    using SceneSerialization = nodec_scene_serialization::SceneSerialization;

public:
    Engine();

    ~Engine() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Destructed.";

        // TODO: Consider to unload all modules before backends.

        // unload all scene entities.
        world_module_->scene().registry().clear();
    }

    void setup();

    void frame_begin() {
        window_->GetGraphics().BeginFrame();
    }

    void frame_end();

    nodec_screen::impl::ScreenModule &screen_module() {
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

    Window &window() {
        return *window_;
    }

    SceneRenderer &scene_renderer() {
        return *scene_renderer_;
    }

private:
    // imgui must be destroyed after window.
    std::unique_ptr<ImguiManager> imgui_manager_;
    std::unique_ptr<Window> window_;

    std::unique_ptr<FontLibrary> font_library_;

    std::unique_ptr<AudioPlatform> audio_platform_;

    std::shared_ptr<nodec_screen::impl::ScreenModule> screen_module_;
    std::unique_ptr<ScreenHandler> screen_handler_;

    std::shared_ptr<nodec_input::InputDevices> input_devices_;
    KeyboardDeviceSystem *keyboard_device_system_;
    MouseDeviceSystem *mouse_device_system_;

    std::shared_ptr<nodec_scene_serialization::SceneLoader> scene_loader_;

    std::shared_ptr<ResourcesModuleBackend> resources_module_;

    std::shared_ptr<SceneSerializationModuleBackend> scene_serialization_module_;

    std::shared_ptr<nodec_world::impl::WorldModule> world_module_;

    std::unique_ptr<SceneRenderer> scene_renderer_;

    std::unique_ptr<nodec_rendering::systems::VisibilitySystem> visibility_system_;

    std::unique_ptr<SceneAudioSystem> scene_audio_system_;

    std::shared_ptr<PhysicsSystemBackend> physics_system_;
};

#if CEREAL_THREAD_SAFE != 1
#    error The macro 'CEREAL_THREAD_SAFE' must be set to '1' for the whole project.
#endif