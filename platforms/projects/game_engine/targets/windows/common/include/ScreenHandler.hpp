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
        using namespace nodec;
        mResolutionChangedConnection = mpScreenModule->resolution_changed().connect(
            [&](ScreenModule& screen, const Vector2i& resolution) {
                screen.internal_resolution = resolution;
            });

        mSizeChangedConnection = mpScreenModule->size_changed().connect(
            [&](ScreenModule& screen, const Vector2i& size) {
                screen.internal_size = size;
            });

        mTitleChangedConnection = mpScreenModule->title_changed().connect(
            [&](ScreenModule& screen, const std::string& title) {
                screen.internal_title = title;
            });

    }

    void OnEngineStarted(NodecEngineModule& engine) {
        assert(mpWindow);

        mResolutionChangedConnection.disconnect();
        mSizeChangedConnection.disconnect();

        mTitleChangedConnection = mpScreenModule->title_changed().connect(
            [&](ScreenModule& screen, const std::string& title) {
                try {
                    mpWindow->SetTitle(title);
                    screen.internal_title = title;
                }
                catch (const std::exception& e) {
                    nodec::logging::ErrorStream(__FILE__, __LINE__)
                        << "[ScreenHandlers] >>> Exception has been occured while Window::SetTitle().\n"
                        << "detail: " << e.what() << std::flush;
                }
                catch (...) {
                    nodec::logging::ErrorStream(__FILE__, __LINE__)
                        << "[ScreenHandlers] >>> Unknown Exception has been occured while Window::SetTitle().\n"
                        << "detail: Unavailable." << std::flush;
                }
            });
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
        mResolutionChangedConnection;

    ScreenModule::SizeChangedSignal::Connection
        mSizeChangedConnection;

    ScreenModule::TitleChangedSignal::Connection
        mTitleChangedConnection;

private:
    NODEC_DISABLE_COPY(ScreenHandler);
};