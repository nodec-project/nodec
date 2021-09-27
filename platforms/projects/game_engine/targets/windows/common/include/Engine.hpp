#pragma once

#include "ScreenHandler.hpp"
#include "Window.hpp"

#include <nodec_engine/impl/nodec_engine_module.hpp>
#include <screen/impl/screen_module.hpp>

#include <nodec/logging.hpp>


class Engine : public nodec_engine::impl::NodecEngineModule {
    //using NodecEngineModule = nodec_engine::impl::NodecEngineModule;
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

        mpScreenModule = &mEngineModule.add_module<ScreenModule>();
        mpScreenHandler.reset(new ScreenHandler(mpScreenModule, mEngineModule));

        mEngineModule.reset();
    }

    void setup(Window* pWindow) {
        mpWindow = pWindow;

        mpScreenHandler->Setup(pWindow);

    }



    ScreenModule& screen_module() { return *screen_module_; }



private
    std::unique_ptr<ScreenModule> screen_module_;

    std::unique_ptr<ScreenHandler> screen_handler_;
    Window* mpWindow{ nullptr };

};