#pragma once

#include "Window.hpp"

#include <nodec_modules/screen/screen_module.hpp>

#include <nodec/macros.hpp>
#include <nodec/event.hpp>

class ScreenHandlers
{
    using ResolutionChangeCallback
        = nodec::event::MemberCallback<
        ScreenHandlers,
        nodec_modules::screen::ScreenModule&, const nodec::Vector2i&
        >;

    using SizeChangeCallback
        = nodec::event::MemberCallback<
        ScreenHandlers,
        nodec_modules::screen::ScreenModule&, const nodec::Vector2i&
        >;

    using TitleChangeCallback
        = nodec::event::MemberCallback<
        ScreenHandlers,
        nodec_modules::screen::ScreenModule&, const std::string&
        >;

public:
    static void SetupOnBootingHandlers(
        std::shared_ptr<ScreenHandlers> handlers,
        nodec_modules::screen::ScreenModule& screenModule
    );

    static void SetupRuntimeHandlers(
        std::shared_ptr<ScreenHandlers> handlers,
        Window* pWindow,
        nodec_modules::screen::ScreenModule& screenModule
    );

public:
    ScreenHandlers();


    void HandleResolutionChangeOnBoot(
        nodec_modules::screen::ScreenModule& screenModule,
        const nodec::Vector2i& resolution
    );

    void HandleSizeChangeOnBoot(
        nodec_modules::screen::ScreenModule& screenModule,
        const nodec::Vector2i& size
    );

    void HandleTitleChangeOnBoot(
        nodec_modules::screen::ScreenModule& screenModule,
        const std::string& title
    );


    void HandleResolutionChangeOnRuntime(
        nodec_modules::screen::ScreenModule& screenModule,
        const nodec::Vector2i& resolution
    );

    void HandleSizeChangeOnRuntime(
        nodec_modules::screen::ScreenModule& screenModule,
        const nodec::Vector2i& size
    );

    void HandleTitleChangeOnRuntime(
        nodec_modules::screen::ScreenModule& screenModule,
        const std::string& title
    );

private:
    Window* pWindow;

    ResolutionChangeCallback::SharedPtr resolutionChangeCallback;

    SizeChangeCallback::SharedPtr sizeChangeCallback;

    TitleChangeCallback::SharedPtr titleChangeCallback;

private:
    NODEC_DISABLE_COPY(ScreenHandlers);
};