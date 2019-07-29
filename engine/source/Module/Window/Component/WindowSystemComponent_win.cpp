#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_WINDOWS)

#include <Windows.h>

#include <AzCore/Application/Application.h>
#include <AzCore/Input/Buses/Notifications/RawInputNotificationBus_win.h>
#include <Window/EBus/WindowSystemComponentBus.h>

extern AZ::Application* gApp;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    switch (uMsg)
    {
    case WM_CLOSE:
        gApp->ExitMainLoop();
        return false;
    case WM_SIZE:
        EBUS_EVENT(Module::WindowsSystemNotificationBus, OnWindowSizeChanged, x, y);
        return false;
    case WM_INPUT:
    {
        UINT rawInputSize;
        const UINT rawInputHeaderSize = sizeof(RAWINPUTHEADER);
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &rawInputSize, rawInputHeaderSize);

        LPBYTE rawInputBytes = new BYTE[rawInputSize];
        const UINT bytesCopied = GetRawInputData((HRAWINPUT)lParam, RID_INPUT, rawInputBytes, &rawInputSize, rawInputHeaderSize);
        RAWINPUT* rawInput = (RAWINPUT*)rawInputBytes;

        EBUS_EVENT(AZ::RawInputNotificationBusWin, OnRawInputEvent, *rawInput);
        return false;
    }
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

void* CreateNativeWindow(int& width, int& height)
{
    WNDCLASS wnd;
    ZeroMemory(&wnd, sizeof(wnd));
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.lpfnWndProc = WndProc;
    wnd.hInstance = GetModuleHandle(nullptr);
    wnd.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wnd.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wnd.lpszClassName = "WindowsLauncher";
    RegisterClass(&wnd);
    
    RECT wr = { 0, 0, width, height };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

    return CreateWindow(
        "WindowsLauncher"
        , "WindowsLauncher"
        , WS_OVERLAPPEDWINDOW | WS_VISIBLE
        , CW_USEDEFAULT
        , CW_USEDEFAULT
        , (wr.right - wr.left)
        , (wr.bottom - wr.top)
        , nullptr
        , nullptr
        , GetModuleHandle(nullptr)
        , nullptr
    );
}

#endif