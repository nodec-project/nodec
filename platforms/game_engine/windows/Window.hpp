#pragma once

#include <nodec/nodec_exception.hpp>
#include <nodec/macros.hpp>

#include <nodec_modules/game_engine/game_engine_module.hpp>

#include <Windows.h>


class Window
{
public:
    class Exception : public nodec::NodecException
    {
        using NodecException::NodecException;
    public:
        const char* type() const noexcept override { return "Window::Exception"; }
        static std::string TranslateErrorCode(HRESULT hr) noexcept;
    };

    class HrException : public Exception
    {
    public:
        HrException(HRESULT hr, const char* file, size_t line) noexcept;
        const char* type() const noexcept override { return "Window::HrException"; }
        HRESULT error_code() const noexcept { return hr; }
    private:
        HRESULT hr;
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
    Window(int width, int height, const wchar_t* name, nodec_modules::game_engine::GameEngineModule* game_engine_module);
    ~Window();

public:
    static bool ProcessMessages(int& exit_code) noexcept;
    void SetTitle(const std::string& title);

private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
    int width;
    int height;
    HWND hWnd;
    nodec_modules::game_engine::GameEngineModule* game_engine_module;


    NODEC_DISABLE_COPY(Window);
};