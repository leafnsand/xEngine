#if X_WINDOWS

#include "Win32Window.h"

#include <EASTL/allocator.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

#include <windowsx.h>

namespace xEngine {

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
  auto self = static_cast<Win32Window *>(GetProp(hwnd, "xEngine"));
  switch (message) {
    case WM_CLOSE: {
      self->config_.delegate->OnWindowClose();
      break;
    }
    case WM_SIZE: {
      self->config_.width = self->config_.frame_buffer_width = LOWORD(lParam);
      self->config_.height = self->config_.frame_buffer_height = HIWORD(lParam);
      self->config_.delegate->OnWindowResize();
      break;
    }
    case WM_MOUSEMOVE: {
      self->mouse_position_.x = GET_X_LPARAM(lParam);
      self->mouse_position_.y = GET_Y_LPARAM(lParam);
      self->config_.delegate->OnWindowMousePosition(self->mouse_position_);
      break;
    }
    case WM_MOUSEWHEEL: {
      self->mouse_scroll_.y = static_cast<int16>(HIWORD(wParam)) / static_cast<float32>(WHEEL_DELTA);
      self->config_.delegate->OnWindowMouseScroll(self->mouse_scroll_);
      break;
    }
    case WM_MOUSEHWHEEL: {
      self->mouse_scroll_.x = -static_cast<int16>(HIWORD(wParam)) / static_cast<float32>(WHEEL_DELTA);
      self->config_.delegate->OnWindowMouseScroll(self->mouse_scroll_);
      break;
    }
    case WM_LBUTTONDOWN: {
      self->mouse_button_down_status_ |= static_cast<uint8>(MouseButtonType::kLeft);
      self->mouse_button_status_cache_ |= static_cast<uint8>(MouseButtonType::kLeft);
      self->config_.delegate->OnWindowMouseButtonDown(MouseButtonType::kLeft);
      break;
    }
    case WM_LBUTTONUP: {
      self->mouse_button_up_status_ |= static_cast<uint8>(MouseButtonType::kLeft);
      self->mouse_button_status_cache_ ^= static_cast<uint8>(MouseButtonType::kLeft);
      self->config_.delegate->OnWindowMouseButtonUp(MouseButtonType::kLeft);
      break;
    }
    case WM_RBUTTONDOWN: {
      self->mouse_button_down_status_ |= static_cast<uint8>(MouseButtonType::kRight);
      self->mouse_button_status_cache_ |= static_cast<uint8>(MouseButtonType::kRight);
      self->config_.delegate->OnWindowMouseButtonDown(MouseButtonType::kRight);
      break;
    }
    case WM_RBUTTONUP: {
      self->mouse_button_up_status_ |= static_cast<uint8>(MouseButtonType::kRight);
      self->mouse_button_status_cache_ ^= static_cast<uint8>(MouseButtonType::kRight);
      self->config_.delegate->OnWindowMouseButtonUp(MouseButtonType::kRight);
      break;
    }
    case WM_MBUTTONDOWN: {
      self->mouse_button_down_status_ |= static_cast<uint8>(MouseButtonType::kMiddle);
      self->mouse_button_status_cache_ |= static_cast<uint8>(MouseButtonType::kMiddle);
      self->config_.delegate->OnWindowMouseButtonDown(MouseButtonType::kMiddle);
      break;
    }
    case WM_MBUTTONUP: {
      self->mouse_button_up_status_ |= static_cast<uint8>(MouseButtonType::kMiddle);
      self->mouse_button_status_cache_ ^= static_cast<uint8>(MouseButtonType::kMiddle);
      self->config_.delegate->OnWindowMouseButtonUp(MouseButtonType::kMiddle);
      break;
    }
    case WM_DROPFILES:
    {
      eastl::vector<eastl::string> files;
      auto drop = reinterpret_cast<HDROP>(wParam);
      auto count = DragQueryFile(drop, 0xFFFFFFFF, nullptr, 0);
      for (auto i = 0; i < count; ++i) {
        auto length = DragQueryFile(drop, i, nullptr, 0) + 1;
        auto buffer = static_cast<char *>(eastl::GetDefaultAllocator()->allocate(length));
        DragQueryFile(drop, i, buffer, length);
        files.push_back(eastl::string(buffer, length));
        eastl::GetDefaultAllocator()->deallocate(buffer, length);
      }
      DragFinish(drop);
      self->config_.delegate->OnWindowDropFile(files);
      break;
    }
    default:
      return DefWindowProc(hwnd, message, wParam, lParam);
  }
  return 0;
}

void Win32Window::Create(const WindowConfig &config) {
  x_assert(config.delegate != nullptr);
  config_ = config;
  window_ = CreateWindow(
    "xEngine",
    config_.title.c_str(),
    config_.is_full_screen ? WS_POPUP : WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    config_.width,
    config_.height,
    nullptr,
    nullptr,
    GetModuleHandle(nullptr),
    nullptr
  );
  if (window_ == nullptr) {
    x_error("fail to create window, code: %d\n", GetLastError());
  }
  SetProp(window_, "xEngine", this);
  ShowWindow(window_, SW_SHOWNORMAL);
  SetCursor(LoadCursorW(nullptr, reinterpret_cast<LPCWSTR>(IDC_ARROW)));
  DragAcceptFiles(window_, config_.is_dropfile_accepted);
  Application::GetInstance().AddLoopDelegate(this);
}

void Win32Window::Destroy() {
  DestroyWindow(window_);
  window_ = nullptr;
  Application::GetInstance().RemoveLoopDelegate(this);
}

void Win32Window::SetTitle(const eastl::string &name) {
  SetWindowText(window_, name.c_str());
}

void Win32Window::OnBeforeEventTick() {
  Reset();
}

void Win32Window::OnAfterEventTick() {
  config_.delegate->OnWindowUpdate();
}

} // namespace xEngine

#endif // X_WINDOWS
