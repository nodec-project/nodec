#pragma once

#include "ScreenHandler.hpp"
#include "Window.hpp"
#include "ImguiManager.hpp"
#include "ResourceLoader.hpp"
#include "ResourcePathHandler.hpp"

#include <nodec_engine/impl/nodec_engine_module.hpp>
#include <screen/impl/screen_module.hpp>
#include <scene_set/impl/scene_module.hpp>
#include <scene_set/systems/transform_system.hpp>
#include <resources/impl/resources_module.hpp>

#include <nodec/logging.hpp>
#include <nodec/unicode.hpp>


class Engine : public nodec_engine::impl::NodecEngineModule {

    using Screen = screen::Screen;
    using ScreenModule = screen::impl::ScreenModule;

    using SceneModule = scene_set::impl::SceneModule;
    using Scene = scene_set::Scene;

    using ResourcesModule = resources::impl::ResourcesModule;
    using Resources = resources::Resources;

public:
    Engine() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Created!";

        nodec_engine::impl::set_current(this);

        imgui_manager_.reset(new ImguiManager);

        // --- screen ---
        screen_module_.reset(new ScreenModule());
        add_module<Screen>(screen_module_);

        screen_handler_.reset(new ScreenHandler(screen_module_.get()));
        screen_handler_->BindHandlersOnBoot();

        // --- scene ---
        scene_module_.reset(new SceneModule());
        add_module<Scene>(scene_module_);

        // --- resources ---
        resources_module_.reset(new ResourcesModule());
        add_module<Resources>(resources_module_);

        resource_path_handler_.reset(new ResourcePathHandler(resources_module_.get()));
        resource_path_handler_->BindHandlersOnBoot();


        initialized().connect([=](NodecEngine&) {
            scene_module_->registry().clear();
                              });
    }

    ~Engine() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Destroyed!";
    }

    void setup() {
        using namespace nodec;

        window_.reset(new Window(screen_module_->internal_size.x, screen_module_->internal_size.y,
                                screen_module_->internal_resolution.x, screen_module_->internal_resolution.y,
                                unicode::utf8to16<std::wstring>(screen_module_->internal_title).c_str()));

        screen_handler_->SetWindow(window_.get());

        resource_path_handler_->BindHandlersOnRuntime();
    }

    void frame_begin() {
        window_->Gfx().BeginFrame();
    }

    void frame_end() {
        using namespace scene_set::systems;

        for (auto& root : scene_module_->hierarchy_system().root_entities()) {
            update_transform(scene_module_->registry(), root);
        }

        window_->Gfx().EndFrame();
    }

    ScreenModule& screen_module() { return *screen_module_; }
    SceneModule& scene_module() { return *scene_module_; }
    ResourcesModule& resources_module() { return *resources_module_; }

private:
    // imgui must be destroyed after window.
    std::unique_ptr<ImguiManager> imgui_manager_;
    std::unique_ptr<Window> window_;

    std::shared_ptr<ScreenModule> screen_module_;
    std::unique_ptr<ScreenHandler> screen_handler_;
    std::shared_ptr<ResourcesModule> resources_module_;
    std::unique_ptr<ResourceLoader> resource_loader_;
    std::unique_ptr<ResourcePathHandler> resource_path_handler_;

    std::shared_ptr<SceneModule> scene_module_;

};