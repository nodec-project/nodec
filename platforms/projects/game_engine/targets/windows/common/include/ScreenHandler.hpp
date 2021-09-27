#pragma once
#include "Window.hpp"

#include <screen/impl/screen_module.hpp>
#include <nodec_engine/impl/nodec_engine_module.hpp>

#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

#include <cassert>


class ScreenHandler {
    using ScreenModule = screen::impl::ScreenModule;
    using NodecEngineModule = nodec_engine::impl::NodecEngineModule;

public:
    ScreenHandler(ScreenModule* pScreenModule, NodecEngineModule& engine)
        : mpScreenModule{ pScreenModule } {
        engine.initialized().connect([&](NodecEngineModule& engine) {OnEngineInitialized(engine); });
        engine.started().connect([&](NodecEngineModule& engine) {OnEngineStarted(engine); });
    }

    void Setup(Window* pWindow) {
        mpWindow = pWindow;
    }


private:
    void OnEngineInitialized(NodecEngineModule& engine) {

    }

    void OnEngineStarted(NodecEngineModule& engine) {
        assert(mpWindow);
    }


    //
    //
    //
    //    void SetupOnBootingHandlers(
    //        & screenModule
    //    );
    //
    //    void SetupRuntimeHandlers(
    //        screen::impl::ScreenModule& screenModule,
    //        Window* pWindow
    //    );
    //
    //public:
    //    ScreenHandlers();
    //
    //
    //    virtual void HandleResolutionChangeOnBoot(
    //        screen::impl::ScreenModule& screenModule,
    //        const nodec::Vector2i& resolution
    //    );
    //
    //    virtual void HandleSizeChangeOnBoot(
    //        screen::impl::ScreenModule& screenModule,
    //        const nodec::Vector2i& size
    //    );
    //
    //    virtual void HandleTitleChangeOnBoot(
    //        screen::impl::ScreenModule& screenModule,
    //        const std::string& title
    //    );
    //
    //
    //    virtual void HandleResolutionChangeOnRuntime(
    //        screen::impl::ScreenModule& screenModule,
    //        const nodec::Vector2i& resolution
    //    );
    //
    //    virtual void HandleSizeChangeOnRuntime(
    //        screen::impl::ScreenModule& screenModule,
    //        const nodec::Vector2i& size
    //    );
    //
    //    virtual void HandleTitleChangeOnRuntime(
    //        screen::impl::ScreenModule& screenModule,
    //        const std::string& title
    //    );

private:
    ScreenModule* mpScreenModule;

    Window* mpWindow{ nullptr };

    ScreenModule::ResolutionChangedSignal::Connection
        resolutionChangedConnection;

    ScreenModule::SizeChangedSignal::Connection
        sizeChangedConnection;

    ScreenModule::TitleChangedSignal::Connection
        titleChangedConnection;

private:
    NODEC_DISABLE_COPY(ScreenHandler);
};