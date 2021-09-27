#pragma once

#include "Window.hpp"

#include <nodec/application.hpp>

#include <memory>


class WinDesktopApplication : public nodec::Application {
public:
    WinDesktopApplication() {}



protected:
    virtual int main_impl() = 0;

    int main() final;
    int on_error_exit() final;

    std::unique_ptr<Window> mpWindow;
    
};