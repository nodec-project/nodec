#include "ScreenHandlers.hpp"

#include <nodec/logging.hpp>

using namespace nodec;

ScreenHandlers::ScreenHandlers() :
    pWindow(nullptr) {
};

void ScreenHandlers::SetupOnBootingHandlers(
    screen::impl::ScreenModule& screenModule
) {
    resolutionChangeConnection = screenModule.resolution_change().connect(
        [&](screen::impl::ScreenModule& screen, const Vector2i& resolution) {
            HandleResolutionChangeOnBoot(screen, resolution);
        }
    );
    sizeChangeConnection = screenModule.size_change().connect(
        [&](screen::impl::ScreenModule& screen, const Vector2i& size) {
            HandleSizeChangeOnBoot(screen, size);
        }
    );
    titleChangeConnection = screenModule.title_change().connect(
        [&](screen::impl::ScreenModule& screen, const std::string& title) {
            HandleTitleChangeOnBoot(screen, title);
        }
    );
}

void ScreenHandlers::SetupRuntimeHandlers(
    screen::impl::ScreenModule& screenModule,
    Window* pWindow
) {
    this->pWindow = pWindow;
    resolutionChangeConnection = screenModule.resolution_change().connect(
        [&](screen::impl::ScreenModule& screen, const Vector2i& resolution) {
            HandleResolutionChangeOnRuntime(screen, resolution);
        }
    );
    sizeChangeConnection = screenModule.size_change().connect(
        [&](screen::impl::ScreenModule& screen, const Vector2i& size) {
            HandleSizeChangeOnRuntime(screen, size);
        }
    );
    titleChangeConnection = screenModule.title_change().connect(
        [&](screen::impl::ScreenModule& screen, const std::string& title) {
            HandleTitleChangeOnRuntime(screen, title);
        }
    );
}


void ScreenHandlers::HandleResolutionChangeOnBoot(
    screen::impl::ScreenModule& screenModule,
    const nodec::Vector2i& resolution
) {
    screenModule.resolution_internal = resolution;
}
void ScreenHandlers::HandleSizeChangeOnBoot(
    screen::impl::ScreenModule& screenModule,
    const nodec::Vector2i& size
) {
    screenModule.size_internal = size;
}
void ScreenHandlers::HandleTitleChangeOnBoot(
    screen::impl::ScreenModule& screenModule,
    const std::string& title
) {
    screenModule.title_internal = title;
}


void ScreenHandlers::HandleResolutionChangeOnRuntime(
    screen::impl::ScreenModule& screenModule,
    const nodec::Vector2i& resolution
) {

}
void ScreenHandlers::HandleSizeChangeOnRuntime(
    screen::impl::ScreenModule& screenModule,
    const nodec::Vector2i& size
) {

}
void ScreenHandlers::HandleTitleChangeOnRuntime(
    screen::impl::ScreenModule& screenModule,
    const std::string& title
) {
    if (pWindow) {
        try {
            pWindow->SetTitle(title);
            screenModule.title_internal = title;
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
    }
}