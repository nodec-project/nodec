#include <nodec/nodec_exception.hpp>

#include <Windows.h>
#include <exception>

#include <nodec_modules/input/keyboard_module.hpp>

#include "Window.hpp"
#include "Utils.hpp"



int CALLBACK WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR     lpCmdLine,
    int       nCmdShow)
{
    try
    {
        nodec_modules::input::KeyboardModule keyboard_module;

        Window window(1280, 720, L"TEST");

        window.SetTitle("ほげってる");
        while (true)
        {
            int exit_code;
            // process all messages pending, but to not block for new messages
            if (!Window::ProcessMessages(exit_code))
            {
                return exit_code;
            }

        }

    }
    catch (const nodec::NodecException& e)
    {
        try
        {
            auto wideWhat = TryMultiByteToWideChar(e.what());
            auto wideType = TryMultiByteToWideChar(e.type());

            MessageBox(nullptr, wideWhat.c_str(), wideType.c_str(), MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const WideCharacterConvertException& e)
        {
            MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONEXCLAMATION);
        }

    }
    catch (const std::exception& e)
    {
        try
        {
            auto wideWhat = TryMultiByteToWideChar(e.what());

            MessageBox(nullptr, wideWhat.c_str(), L"StandardException", MB_OK | MB_ICONEXCLAMATION);
        }
        catch (const WideCharacterConvertException& e)
        {
            MessageBox(nullptr, e.what(), e.type(), MB_OK | MB_ICONEXCLAMATION);
        }
    }
    catch (...)
    {
        MessageBox(nullptr, L"Unknow Error Occurs. No details available.", L"UnkownErrorException", MB_OK | MB_ICONEXCLAMATION);
    }
    
    return -1;
}
