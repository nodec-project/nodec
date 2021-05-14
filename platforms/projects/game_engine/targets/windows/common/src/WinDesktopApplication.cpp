/**
*
*/

#include "WinDesktopApplication.hpp"
#include "Logging.hpp"


#include <nodec/logging.hpp>

#include <Windows.h>


int WinDesktopApplication::main()
{
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


    auto exitCode = main_impl();


    nodec::logging::InfoStream(__FILE__, __LINE__) 
        << "[Main] >>> Application Sucessfully Ending. See you." << std::flush;

    return exitCode;
}

int WinDesktopApplication::on_error_exit()
{
    try
    {
        throw;
    }
    catch (const std::exception& e)
    {
        nodec::logging::fatal(e.what(), __FILE__, __LINE__);
    }
    catch (...)
    {
        nodec::logging::fatal("Unknown Error Exception Ocuurs.", __FILE__, __LINE__);
    }

    MessageBox(nullptr,
               L"Unhandled Exception has been caught in main loop. \nFor more detail, Please check the 'output.log'.",
               L"Fatal Error.", MB_OK | MB_ICONEXCLAMATION);

    nodec::logging::WarnStream(__FILE__, __LINE__) 
        << "[Main] >>> Unexpected Program Ending." << std::flush;

    return -1;
}
