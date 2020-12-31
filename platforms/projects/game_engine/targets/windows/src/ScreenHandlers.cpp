#include "ScreenHandlers.hpp"

#include <nodec/logging.hpp>

ScreenHandlers::ScreenHandlers() :
    pWindow(nullptr)
{
};

void ScreenHandlers::SetupOnBootingHandlers(
    std::shared_ptr<ScreenHandlers> handlers,
    nodec_modules::screen::ScreenModule& screenModule
)
{
    if (handlers->resolutionChangeCallback)
    {
        screenModule.on_submit_resolution_change -= handlers->resolutionChangeCallback;
    }

    if (handlers->sizeChangeCallback)
    {
        screenModule.on_submit_size_change -= handlers->sizeChangeCallback;
    }

    if (handlers->titleChangeCallback)
    {
        screenModule.on_submit_title_change -= handlers->titleChangeCallback;
    }

    handlers->resolutionChangeCallback = ResolutionChangeCallback::make_shared(handlers, &ScreenHandlers::HandleResolutionChangeOnBoot);
    handlers->sizeChangeCallback = SizeChangeCallback::make_shared(handlers, &ScreenHandlers::HandleSizeChangeOnBoot);
    handlers->titleChangeCallback = TitleChangeCallback::make_shared(handlers, &ScreenHandlers::HandleTitleChangeOnBoot);

    screenModule.on_submit_resolution_change += handlers->resolutionChangeCallback;
    screenModule.on_submit_size_change += handlers->sizeChangeCallback;
    screenModule.on_submit_title_change += handlers->titleChangeCallback;
}

void ScreenHandlers::SetupRuntimeHandlers(
    std::shared_ptr<ScreenHandlers> handlers, 
    Window* pWindow,
    nodec_modules::screen::ScreenModule& screenModule
)
{
    if (handlers->resolutionChangeCallback)
    {
        screenModule.on_submit_resolution_change -= handlers->resolutionChangeCallback;
    }

    if (handlers->sizeChangeCallback)
    {
        screenModule.on_submit_size_change -= handlers->sizeChangeCallback;
    }

    if (handlers->titleChangeCallback)
    {
        screenModule.on_submit_title_change -= handlers->titleChangeCallback;
    }
    handlers->pWindow = pWindow;

    handlers->resolutionChangeCallback = ResolutionChangeCallback::make_shared(handlers, &ScreenHandlers::HandleResolutionChangeOnRuntime);
    handlers->sizeChangeCallback = SizeChangeCallback::make_shared(handlers, &ScreenHandlers::HandleSizeChangeOnRuntime);
    handlers->titleChangeCallback = TitleChangeCallback::make_shared(handlers, &ScreenHandlers::HandleTitleChangeOnRuntime);

    screenModule.on_submit_resolution_change += handlers->resolutionChangeCallback;
    screenModule.on_submit_size_change += handlers->sizeChangeCallback;
    screenModule.on_submit_title_change += handlers->titleChangeCallback;
}


void ScreenHandlers::HandleResolutionChangeOnBoot(
    nodec_modules::screen::ScreenModule& screenModule,
    const nodec::Vector2i& resolution
)
{
    screenModule.resolution_ = resolution;
}
void ScreenHandlers::HandleSizeChangeOnBoot(
    nodec_modules::screen::ScreenModule& screenModule,
    const nodec::Vector2i& size
)
{
    screenModule.size_ = size;
}
void ScreenHandlers::HandleTitleChangeOnBoot(
    nodec_modules::screen::ScreenModule& screenModule,
    const std::string& title
)
{
    screenModule.title_ = title;
}


void ScreenHandlers::HandleResolutionChangeOnRuntime(
    nodec_modules::screen::ScreenModule& screenModule, 
    const nodec::Vector2i& resolution
)
{

}
void ScreenHandlers::HandleSizeChangeOnRuntime(
    nodec_modules::screen::ScreenModule& screenModule, 
    const nodec::Vector2i& size
)
{

}
void ScreenHandlers::HandleTitleChangeOnRuntime(
    nodec_modules::screen::ScreenModule& screenModule, 
    const std::string& title
)
{
    if (pWindow)
    {
        try
        {
            pWindow->SetTitle(title);
        }
        catch (const nodec::NodecException& e)
        {
            nodec::logging::ErrorStream(__FILE__, __LINE__)
                << "[ScreenHandlers] >>> Nodec Exception has been occured while Window::SetTitle().\n"
                << "detail: " << e.what() << std::flush;
        }
        catch (const std::exception& e)
        {
            nodec::logging::ErrorStream(__FILE__, __LINE__)
                << "[ScreenHandlers] >>> Std Exception has been occured while Window::SetTitle().\n"
                << "detail: " << e.what() << std::flush;
        }
        catch (...)
        {
            nodec::logging::ErrorStream(__FILE__, __LINE__)
                << "[ScreenHandlers] >>> Unknown Exception has been occured while Window::SetTitle().\n"
                << "detail: Unavailable." << std::flush;
        }
    }
}