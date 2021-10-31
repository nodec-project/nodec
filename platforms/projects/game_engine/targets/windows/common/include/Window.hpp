#pragma once

#include "Graphics/Graphics.hpp"

#include <nodec/error_formatter.hpp>
#include <nodec/macros.hpp>
#include <nodec/signals.hpp>

#define NOMINMAX
#include <Windows.h>

#include <stdexcept>


class Window
{
public:
    class HrException : public std::runtime_error
    {
    public:
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
        
    public:
        HrException(HRESULT hr, const char* file, size_t line) noexcept
            : errorCode(hr)
            , runtime_error(nodec::error_fomatter::with_type_file_line<HrException>(
                nodec::Formatter()
                << "[Error Code] 0x" << std::hex << std::uppercase << hr << std::dec
                << " (" << (unsigned long)hr << ")\n"
                << "[Description] " << TranslateErrorCode(hr),
                file, line)) {
        };

        HRESULT ErrorCode() const noexcept { return errorCode; }

    private:
        const HRESULT errorCode;
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

    private:
        NODEC_DISABLE_COPY(WindowClass);
    };

public:
    Window(int width, int height, 
           int gfxWidth, int gfxHeight, 
           const wchar_t* name);

    ~Window();

public:
    static bool ProcessMessages(int& exit_code) noexcept;
    void SetTitle(const std::string& title);

    Graphics& Gfx();

public:
    using WindowSignal = nodec::signals::Signal<void(Window&)>;
    
    decltype(auto) WindowDestroyed() { return mWindowDestroyed.signal_interface(); }

private:
    WindowSignal mWindowDestroyed;

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
    int width;
    int height;
    HWND hWnd;
    std::unique_ptr<Graphics> pGfx;

private:
    NODEC_DISABLE_COPY(Window);
};