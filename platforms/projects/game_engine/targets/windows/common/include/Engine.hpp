#pragma once

#include "ScreenHandler.hpp"
#include "Window.hpp"
#include "ImguiManager.hpp"

#include <nodec_engine/impl/nodec_engine_module.hpp>
#include <screen/impl/screen_module.hpp>

#include <nodec/logging.hpp>
#include <nodec/unicode.hpp>


class Engine : public nodec_engine::impl::NodecEngineModule {
    using Screen = screen::Screen;
    using ScreenModule = screen::impl::ScreenModule;

public:
    Engine() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Created!";

    }

    ~Engine() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine] >>> Destroyed!";

    }

    void configure() {
        nodec::logging::InfoStream(__FILE__, __LINE__) << "[Engine::Configure()]";

        nodec_engine::impl::set_current(this);

        imgui_manager_.reset(new ImguiManager);

        screen_module_.reset(new ScreenModule());
        register_module<Screen>(screen_module_.get());

        screen_handler_.reset(new ScreenHandler(screen_module_.get(), *this));


        reset();
    }

    void setup() {
        using namespace nodec;

        window_.reset(new Window(screen_module_->internal_resolution.x, screen_module_->internal_resolution.y,
                                screen_module_->internal_resolution.x, screen_module_->internal_resolution.y,
                                unicode::utf8to16<std::wstring>(screen_module_->internal_title).c_str()));

        screen_handler_->Setup(window_.get());
    }

    void frame_begin() {
        window_->Gfx().BeginFrame();
    }

    void frame_end() {
        window_->Gfx().EndFrame();
    }

    ScreenModule& screen_module() { return *screen_module_; }

private:
    // imgui must be destroyed after window.
    std::unique_ptr<ImguiManager> imgui_manager_;
    std::unique_ptr<Window> window_;

    std::unique_ptr<ScreenModule> screen_module_;

    std::unique_ptr<ScreenHandler> screen_handler_;

};