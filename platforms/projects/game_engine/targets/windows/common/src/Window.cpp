#include "Window.hpp"
#include "Utils.hpp"
#include "imgui_backend/imgui_impl_win32.h"
#include "imgui_backend/imgui_impl_dx11.h"

#include <nodec/unicode.hpp>

#include <string>
#include <sstream>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


// === Window Class ====

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass()
    :
    hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = { 0 };
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

Window::Window(int width, int height,
               int gfxWidth, int gfxHeight,
               const wchar_t* name,
               input::keyboard::impl::KeyboardModule* keyboardModule,
               input::mouse::impl::MouseModule* mouseModule)
    :
    width(width),
    height(height),
    keyboardModule(keyboardModule),
    mouseModule(mouseModule)
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

    // create window & get hWnd.
    hWnd = CreateWindow(
        WindowClass::GetName(), name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, WindowClass::GetInstance(), this
    );

    // create for error.
    if (hWnd == nullptr)
    {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }

    // newly created windows start off as hidden.
    ShowWindow(hWnd, SW_SHOWDEFAULT);

    // create graphics object
    pGfx = std::make_unique<Graphics>(hWnd, gfxWidth, gfxHeight);


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Init ImGUI Win32 Impl
    ImGui_ImplWin32_Init(hWnd);
    // init imgui d3d impl
    ImGui_ImplDX11_Init(pGfx->GetDevice(), pGfx->GetContext());
}

Window::~Window()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(hWnd);
    
    nodec::logging::InfoStream(__FILE__, __LINE__) << "[Window] >>> End Window." << std::flush;
}

void Window::SetTitle(const std::string& title)
{
    //std::string wideTitleBytes = nodec::unicode::utf8to16(title);

    ////nodec::logging::DebugStream(__FILE__, __LINE__) << title.size() << ", " << wideTitleBytes.size() << std::flush;

    //std::wstring wideTitle(reinterpret_cast<wchar_t*>(&wideTitleBytes[0]), wideTitleBytes.size() / 2);

    std::wstring wideTitle;
    Utils::UTF8ToUTF16WString(title, wideTitle);

    if (SetWindowText(hWnd, wideTitle.c_str()) == 0)
    {
        throw HrException(GetLastError(), __FILE__, __LINE__);
    }
}

Graphics& Window::Gfx()
{
    if (!pGfx)
    {
        throw NoGfxException(__FILE__, __LINE__);
    }
    return *pGfx;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side.
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data.
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
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

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    // retrieve ptr to window instance
    //  pointer is const, value not const
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window instance handler
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
        // We don't want the DefProc to handle this message because
        // we want our destructor to destoroy the window, so return 0 instead of break.
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

        // when window loses funcs to prevent input
    case WM_KILLFOCUS:
        //SetTitle("A");
        break;

        // on window to foreground/background
    case WM_ACTIVATE:
        if (wParam & WA_ACTIVE)
        {
            // active by keyboard not mouse
            //SetTitle("b");

        }
        else
        {
            // active by mouse
            //SetTitle("c");
        }
        break;

        // --- KEYBOARD MESSAGE ---
    case WM_KEYDOWN:
        // syskey commands need to be handled to track ALT key (VK_MENU) and F10
    case WM_SYSKEYDOWN:
        // filter autorepeat
        if (!(lParam & 0x40000000))
        {
            keyboardModule->handle_key_press(static_cast<input::keyboard::Key>(wParam));
        }
        break;

    case WM_KEYUP:
    case WM_SYSKEYUP:

        keyboardModule->handle_key_release(static_cast<input::keyboard::Key>(wParam));

        break;

    case WM_CHAR:

        keyboardModule->handle_text_input(static_cast<unsigned char>(wParam));

        break;

        // END KEYBOARD MESSAGE ---

        // --- MOUSE MESSAGE ---
    case WM_MOUSEMOVE:
    {
        const POINTS pt = MAKEPOINTS(lParam);

        mouseModule->handle_mouse_move({ pt.x, pt.y });

        break;
    }
    case WM_LBUTTONDOWN:
    {
        const POINTS pt = MAKEPOINTS(lParam);
        
        mouseModule->handle_button_press(input::mouse::MouseButton::Left,
                                         { pt.x, pt.y });

        break;
    }
    case WM_RBUTTONDOWN:
    {
        const POINTS pt = MAKEPOINTS(lParam);

        mouseModule->handle_button_press(input::mouse::MouseButton::Right,
                                         { pt.x, pt.y });

        break;
    }
    case WM_LBUTTONUP:
    {
        const POINTS pt = MAKEPOINTS(lParam);

        mouseModule->handle_button_release(input::mouse::MouseButton::Left,
                                         { pt.x, pt.y });

        break;
    }
    case WM_RBUTTONUP:
    {
        const POINTS pt = MAKEPOINTS(lParam);

        mouseModule->handle_button_release(input::mouse::MouseButton::Right,
                                           { pt.x, pt.y });

        break;
    }
    case WM_MOUSEWHEEL:
    {

        break;
    }
    // END MOUSE MESSAGE ---

    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool Window::ProcessMessages(int& exit_code) noexcept
{
    MSG msg;

    // while queue has message, remove and dispatch them (but do not block on empty queue)
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
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