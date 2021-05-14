#pragma once

#include "WinDesktopApplication.hpp"


class Application : public WinDesktopApplication {

public:
    Application() {};

protected:
    int main_impl() final;
};