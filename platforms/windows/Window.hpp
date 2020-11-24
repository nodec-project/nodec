#pragma once
#include <Windows.h>

class Window {
private:
    /**
    * @brief Singleton class.
    */
    class WindowClass {
    public:
        static const TCHAR* GetName();
        static HINSTANCE GetInstance();
    private:
        WindowClass();
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;
        static constexpr const char* wndClassName = "Nodec Engine Window";
        static WindowClass wndClass;
        HINSTANCE hInst;
    };

public:
    Window(int width, int height, const TCHAR* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
    int width;
    int height;
    HWND hWnd;
};