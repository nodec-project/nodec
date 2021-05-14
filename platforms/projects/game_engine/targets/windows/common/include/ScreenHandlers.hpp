#pragma once

#include "Window.hpp"

#include <screen/impl/screen_module.hpp>

#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

class ScreenHandlers {
public:
    void SetupOnBootingHandlers(
        screen::impl::ScreenModule& screenModule
    );

    void SetupRuntimeHandlers(
        screen::impl::ScreenModule& screenModule,
        Window* pWindow
    );

public:
    ScreenHandlers();


    virtual void HandleResolutionChangeOnBoot(
        screen::impl::ScreenModule& screenModule,
        const nodec::Vector2i& resolution
    );

    virtual void HandleSizeChangeOnBoot(
        screen::impl::ScreenModule& screenModule,
        const nodec::Vector2i& size
    );

    virtual void HandleTitleChangeOnBoot(
        screen::impl::ScreenModule& screenModule,
        const std::string& title
    );


    virtual void HandleResolutionChangeOnRuntime(
        screen::impl::ScreenModule& screenModule,
        const nodec::Vector2i& resolution
    );

    virtual void HandleSizeChangeOnRuntime(
        screen::impl::ScreenModule& screenModule,
        const nodec::Vector2i& size
    );

    virtual void HandleTitleChangeOnRuntime(
        screen::impl::ScreenModule& screenModule,
        const std::string& title
    );

private:
    Window* pWindow;

    screen::impl::ScreenModule::ResolutionChangeSignal::Connection
        resolutionChangeConnection;

    screen::impl::ScreenModule::SizeChangeSignal::Connection
        sizeChangeConnection;

    screen::impl::ScreenModule::TitleChangeSignal::Connection
        titleChangeConnection;

private:
    NODEC_DISABLE_COPY(ScreenHandlers);
};