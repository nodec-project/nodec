#pragma once

#include "Window.hpp"
#include "Logging.hpp"

#include <nodec/application.hpp>
#include <nodec/logging.hpp>

#define NOMINMAX
#include <Windows.h>

#include <memory>


class WinDesktopApplication : public nodec::Application {
public:
    WinDesktopApplication() {}


protected:

    virtual void setup() = 0;
    virtual void loop() = 0;


    int main() final {
        // --- Init Logging ---
        InitLogging(nodec::logging::Level::Debug);

        //
        //#ifndef NDEBUG
        //    InitLogging(nodec::logging::Level::Debug);
        //#else
        //    InitLogging(nodec::logging::Level::Info);
        //#endif
        // end Init Logging ---


        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[Main] >>> Hello world. Application start." << std::flush;

        setup();

        int exitCode;
        while (true) {
            if (!Window::ProcessMessages(exitCode)) {
                break;
            }

            loop();
        }

        nodec::logging::InfoStream(__FILE__, __LINE__)
            << "[Main] >>> Application Successfully Ending. See you." << std::flush;

        return exitCode;
    }

    int on_error_exit() final {
        try {
            throw;
        }
        catch (const std::exception& e) {
            nodec::logging::fatal(e.what(), __FILE__, __LINE__);
        }
        catch (...) {
            nodec::logging::fatal("Unknown Error Exception Occurs.", __FILE__, __LINE__);
        }

        MessageBox(nullptr,
                   L"Unhandled Exception has been caught in main loop. \nFor more detail, Please check the 'output.log'.",
                   L"Fatal Error.", MB_OK | MB_ICONEXCLAMATION);

        nodec::logging::WarnStream(__FILE__, __LINE__)
            << "[Main] >>> Unexpected Program Ending." << std::flush;

        return -1;
    }
};