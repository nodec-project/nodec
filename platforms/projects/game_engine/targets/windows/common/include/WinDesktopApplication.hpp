#pragma once

#include <nodec/application.hpp>

class WinDesktopApplication : public nodec::Application {
public:
    WinDesktopApplication() {
    }

protected:
    virtual int main_impl() = 0;

    int main() final;
    int on_error_exit() final;
};