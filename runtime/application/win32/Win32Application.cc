#if X_WINDOWS

#include "core/Core.h"

#include "application/ApplicationDelegate.h"

#include <Windows.h>

namespace xEngine {

extern LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

void NativeApplicationInitialize(ApplicationDelegate *delegate) {
  WNDCLASS wndClass;
  ZeroMemory(&wndClass, sizeof(wndClass));
  wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wndClass.lpfnWndProc = WndProc;
  wndClass.hInstance = GetModuleHandle(nullptr);
  wndClass.lpszClassName = "xEngine";
  wndClass.hIcon = static_cast<HICON>(LoadImage(GetModuleHandle(nullptr), "xEngineIcon", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
  if (!wndClass.hIcon) {
    wndClass.hIcon = static_cast<HICON>(LoadImage(nullptr, IDI_APPLICATION, IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED));
  }
  if (!RegisterClass(&wndClass)) {
    x_error("register class error, code: %d\n", GetLastError());
  }
  delegate->Initialize();
}

void NativeApplicationFinalize(ApplicationDelegate *delegate) {
  delegate->Finalize();
  UnregisterClass("xEngine", GetModuleHandle(nullptr));
  PostQuitMessage(0);
}

bool PollNativeApplicationEvent() {
  MSG msg;
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    if (msg.message == WM_QUIT) {
      return true;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return false;
}

} // namespace xEngine

#endif // X_WINDOWS