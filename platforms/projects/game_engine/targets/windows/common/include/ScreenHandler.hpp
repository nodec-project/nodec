#pragma once

#include "Window.hpp"

#include <nodec_screen/impl/screen_module.hpp>
#include <nodec_engine/impl/nodec_engine_module.hpp>

#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

#include <cassert>


class ScreenHandler {
    using ScreenModule = nodec_screen::impl::ScreenModule;

public:
    ScreenHandler(ScreenModule* pScreenModule)
        : mpScreenModule{ pScreenModule } {
    }

    void Setup(Window* pWindow) {
        mpWindow = pWindow;
    }


public:
    void BindHandlersOnBoot() {
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

    void BindHandlersOnRuntime() {

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
                        << "[ScreenHandlers] >>> Exception has been occurred while Window::SetTitle().\n"
                        << "detail: " << e.what() << std::flush;
                }
                catch (...) {
                    nodec::logging::ErrorStream(__FILE__, __LINE__)
                        << "[ScreenHandlers] >>> Unknown Exception has been occurred while Window::SetTitle().\n"
                        << "detail: Unavailable." << std::flush;
                }
            });
    }

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
    NODEC_DISABLE_COPY(ScreenHandler)
};