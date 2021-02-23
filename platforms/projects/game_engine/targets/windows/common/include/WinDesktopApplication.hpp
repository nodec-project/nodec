#pragma once

#include <game_engine/impl/game_engine_module.hpp>

#include <nodec/application.hpp>

class WinDesktopApplication : public nodec::Application
{
protected:
    

public:
    WinDesktopApplication()
    {
    }


protected:
    virtual int main_impl() = 0;

    int main() final;
    int on_error_exit() final;


};