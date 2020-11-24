#include "Window.hpp"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
    :
    hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    //wc.lpfnWndProc = 
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    RegisterClassEx(&wc);
}


Window::Window(int width, int height, const TCHAR* name)
    :
    width(width),
    height(height) {
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw;
    }

    // create window & get hWnd
    hWnd = CreateWindow(
        WindowClass::GetName(), name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, WindowClass::GetInstance(), this
    );

    if (hWnd == nullptr) {
        throw;
    }
    ShowWindow(hWnd, SW_SHOWDEFAULT);
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

}