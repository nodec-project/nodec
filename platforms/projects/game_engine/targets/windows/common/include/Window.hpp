#pragma once

#include "Graphics/Graphics.hpp"

#include <input/keyboard/impl/keyboard_module.hpp>
#include <input/mouse/impl/mouse_module.hpp>

#include <nodec/nodec_exception.hpp>
#include <nodec/macros.hpp>

#include <Windows.h>


class Window
{
public:
    class Exception : public nodec::NodecException
    {
        using NodecException::NodecException;
    public:
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
    };

    class HrException : public Exception
    {
    public:
        HrException(HRESULT hr, const char* file, size_t line) noexcept;
        HRESULT error_code() const noexcept { return hr; }
    private:
        HRESULT hr;
    };

    class NoGfxException : public Exception
    {
    public:
        NoGfxException(const char* file, size_t line) :
            Exception("No Graphics.", file, line)
        {
        }
    };

private:
    /**
    * @brief Singleton class. Manage registration/cleanup of window class.
    */
    class WindowClass
    {
    public:
        static const wchar_t* GetName() noexcept;
        static HINSTANCE GetInstance() noexcept;

    private:
        WindowClass();
        ~WindowClass();
        static constexpr const wchar_t* wndClassName = L"Nodec Engine Window";
        static WindowClass wndClass;
        HINSTANCE hInst;

        NODEC_DISABLE_COPY(WindowClass);
    };

public:
    Window(int width, int height, 
           int gfxWidth, int gfxHeight, 
           const wchar_t* name, 
           input::keyboard::impl::KeyboardModule* keyboardModule,
           input::mouse::impl::MouseModule* mouseModule
           );

    ~Window();

public:
    static bool ProcessMessages(int& exit_code) noexcept;
    void SetTitle(const std::string& title);

    Graphics& Gfx();

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
    int width;
    int height;
    HWND hWnd;
    input::keyboard::impl::KeyboardModule* keyboardModule;
    input::mouse::impl::MouseModule* mouseModule;
    std::unique_ptr<Graphics> pGfx;

    NODEC_DISABLE_COPY(Window);
};