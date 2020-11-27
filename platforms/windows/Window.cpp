#include "Window.hpp"

#include <nodec/unicode.hpp>

#include <sstream>

// === Window Class ====

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
    :
    hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = DefWindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = nullptr;
    wc.hIconSm = nullptr;
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();

    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

// END Window Class ===

Window::Window(int width, int height, const wchar_t* name)
    :
    width(width),
    height(height)
{
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }

    // create window & get hWnd
    hWnd = CreateWindow(
        WindowClass::GetName(), name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, WindowClass::GetInstance(), this
    );

    if (hWnd == nullptr)
    {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }
    ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
    DestroyWindow(hWnd);
}
LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}


/**
* <https://docs.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages>
*/
std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    char* pMsgBuf = nullptr;

    // windows will allocate memory for err string and make point to it

    const DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, 0,
        reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr
    );

    // 0 string length returned indicates a failure
    if (nMsgLen == 0)
    {
        return "Undefined error code";
    }

    // copy error string from windows-allocated buffer to std::string
    std::string errorString = nodec::unicode::utf16to8(pMsgBuf);
    LocalFree(pMsgBuf);
    return errorString;
}

Window::HrException::HrException(HRESULT hr, const char* file, size_t line) noexcept
    :
    Exception(file, line),
    hr(hr)
{
    std::ostringstream oss;
    oss << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
        << " (" << (unsigned long)hr << ")" << std::endl
        << "[Description] " << Exception::TranslateErrorCode(hr) << std::endl;

    message = oss.str();
}