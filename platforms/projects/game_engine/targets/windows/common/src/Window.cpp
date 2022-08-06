#include "Window.hpp"
#include "imgui_impl_win32.h"

#include <nodec/unicode.hpp>

#include <sstream>
#include <string>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// === Window Class ====

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
    : hInst(GetModuleHandle(nullptr)) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = HandleMsgSetup;
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

Window::WindowClass::~WindowClass() {
    UnregisterClass(wndClassName, GetInstance());
}

const wchar_t *Window::WindowClass::GetName() noexcept {
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
    return wndClass.hInst;
}

// END Window Class ===

Window::Window(int width, int height,
               int gfxWidth, int gfxHeight,
               const wchar_t *name,
               nodec_input::impl::InputModule *pInputModule)
    : mWidth(width), mHeight(height), mpInputModule(pInputModule) {
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0) {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }

    // create window & get hWnd.
    hWnd = CreateWindow(
        WindowClass::GetName(), name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, WindowClass::GetInstance(), this);

    // create for error.
    if (hWnd == nullptr) {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }

    // newly created windows start off as hidden.
    ShowWindow(hWnd, SW_SHOWDEFAULT);

    // create graphics object
    mpGraphics = std::make_unique<Graphics>(hWnd, gfxWidth, gfxHeight);

    // Init ImGUI Win32 Impl
    ImGui_ImplWin32_Init(hWnd);
}

Window::~Window() {
    // Cleanup
    ImGui_ImplWin32_Shutdown();

    mWindowDestroyed(*this);

    DestroyWindow(hWnd);

    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Window] >>> End Window." << std::flush;
}

void Window::SetTitle(const std::string &title) {
    std::wstring titleW = nodec::unicode::utf8to16<std::wstring>(title);

    if (SetWindowText(hWnd, titleW.c_str()) == 0) {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side.
    if (msg == WM_NCCREATE) {
        // extract ptr to window class from creation data.
        const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
        Window *const pWnd = static_cast<Window *>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window instance.
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished.
        // @note:
        //  Why don't call the handler directly like this?
        //  >> SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(pWnd->HandleMsg);
        //
        //  It's not possible. member function include the this pointer.
        //  Cannot reference to member func using only member function pointer.
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // forward message to window instance handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    // if we get a message before the WM_NCREATE message, handle with default handler.
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    // retrieve ptr to window instance
    //  pointer is const, value not const
    Window *const pWnd = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window instance handler
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
        return true;
    }

    const auto &imio = ImGui::GetIO();

    switch (msg) {
        // We don't want the DefProc to handle this message because
        // we want our destructor to destoroy the window, so return 0 instead of break.
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

        //    // when window loses funcs to prevent input
        // case WM_KILLFOCUS:
        //    //SetTitle("A");
        //    break;

        //    // on window to foreground/background
        // case WM_ACTIVATE:
        //    if (wParam & WA_ACTIVE) {
        //        // active by keyboard not mouse
        //        //SetTitle("b");

        //    }
        //    else {
        //        // active by mouse
        //        //SetTitle("c");
        //    }
        //    break;

        // --- KEYBOARD MESSAGE ---
    case WM_KEYDOWN:
        // syskey commands need to be handled to track ALT key (VK_MENU) and F10
    case WM_SYSKEYDOWN: {
        if (imio.WantCaptureKeyboard) break;

        // nodec::logging::InfoStream(__FILE__, __LINE__) << lParam << ", " << wParam;

        // filter autorepeat
        // if (!(lParam & 0x40000000)) {

        using namespace nodec_input::keyboard;
        mpInputModule->keyboard_impl().key_event_impl()(
            KeyEvent{
                KeyEvent::Type::Press,
                static_cast<Key>(wParam)});

    } break;

    case WM_KEYUP:
    case WM_SYSKEYUP: {
        if (imio.WantCaptureKeyboard) break;

        using namespace nodec_input::keyboard;
        mpInputModule->keyboard_impl().key_event_impl()(
            KeyEvent{
                KeyEvent::Type::Release,
                static_cast<Key>(wParam)});

    } break;

        // case WM_CHAR:

        //    keyboardModule->handle_text_input(static_cast<unsigned char>(wParam));

        //    break;

        //    // END KEYBOARD MESSAGE ---

        //    // --- MOUSE MESSAGE ---
        // case WM_MOUSEMOVE:
        //{
        //    const POINTS pt = MAKEPOINTS(lParam);

        //    mouseModule->handle_mouse_move({ pt.x, pt.y });
        //    /*if (ImGui::GetCurrentContext()) {

        //        ImGui::GetIO().MousePos = ImVec2(pt.x / 1.5, pt.y / 1.5);
        //    }*/
        //    break;
        //}
        // case WM_LBUTTONDOWN:
        //{
        //    const POINTS pt = MAKEPOINTS(lParam);

        //    mouseModule->handle_button_press(input::mouse::MouseButton::Left,
        //                                     { pt.x, pt.y });

        //    break;
        //}
        // case WM_RBUTTONDOWN:
        //{
        //    const POINTS pt = MAKEPOINTS(lParam);

        //    mouseModule->handle_button_press(input::mouse::MouseButton::Right,
        //                                     { pt.x, pt.y });

        //    break;
        //}
        // case WM_LBUTTONUP:
        //{
        //    const POINTS pt = MAKEPOINTS(lParam);

        //    mouseModule->handle_button_release(input::mouse::MouseButton::Left,
        //                                       { pt.x, pt.y });

        //    break;
        //}
        // case WM_RBUTTONUP:
        //{
        //    const POINTS pt = MAKEPOINTS(lParam);

        //    mouseModule->handle_button_release(input::mouse::MouseButton::Right,
        //                                       { pt.x, pt.y });

        //    break;
        //}
        // case WM_MOUSEWHEEL:
        //{

        //    break;
        //}
        // END MOUSE MESSAGE ---
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::ProcessMessages(int &exit_code) noexcept {
    MSG msg;

    // while queue has message, remove and dispatch them (but do not block on empty queue)
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            exit_code = (int)msg.wParam;
            // signals quit
            return false;
        }

        // TranslateMessage will post auxiliary WM_CHAR messages from key msgs
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // return true when not quitting app
    return true;
}

/**
 * <https://docs.microsoft.com/en-us/windows/win32/seccrypto/retrieving-error-messages>
 */
std::string Window::HrException::TranslateErrorCode(HRESULT hr) noexcept {
    char *pMsgBuf = nullptr;

    // windows will allocate memory for err string and make point to it

    const DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, 0,
        reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);

    // 0 string length returned indicates a failure
    if (nMsgLen == 0) {
        return "Undefined error code";
    }

    // copy error string from windows-allocated buffer to std::string
    std::string errorString = nodec::unicode::utf16to8<std::string>(std::string(pMsgBuf));
    LocalFree(pMsgBuf);
    return errorString;
}
