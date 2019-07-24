#ifndef XENGINE_WINDOW_MACOS_MACOSWINDOW_H
#define XENGINE_WINDOW_MACOS_MACOSWINDOW_H

#if X_MACOS

#include "window/WindowInterface.h"
#include "application/ApplicationDelegate.h"

namespace xEngine {

class MacOSWindow: public WindowInterface, ApplicationTickDelegate {
 public:
  virtual void Create(const WindowConfig &config) override;

  virtual void Destroy() override;

  virtual void SetTitle(const eastl::string &name) override;

  virtual void *GetNativeHandle() override { return window_; }

  virtual void OnBeforeEventTick() override;

  virtual void OnAfterEventTick() override;

 private:
  void *window_{nullptr};

  friend WindowConfig &GetConfig(MacOSWindow *window) {
    return window->config_;
  }

  friend void SetMousePosition(MacOSWindow *window, float32 x, float32 y) {
    window->mouse_position_.x = x;
    window->mouse_position_.y = y;
    window->config_.delegate->OnWindowMousePosition(window->mouse_position_);
  }

  friend void SetMouseScroll(MacOSWindow *window, float32 x, float32 y) {
    window->mouse_scroll_.x = x;
    window->mouse_scroll_.y = y;
    window->config_.delegate->OnWindowMouseScroll(window->mouse_scroll_);
  }

  friend void SetMouseButtonDown(MacOSWindow *window, MouseButtonType type) {
    window->mouse_button_down_status_ |= static_cast<uint8>(type);
    window->mouse_button_status_cache_ |= static_cast<uint8>(type);
    window->config_.delegate->OnWindowMouseButtonDown(type);
  }

  friend void SetMouseButtonUp(MacOSWindow *window, MouseButtonType type) {
    window->mouse_button_up_status_ |= static_cast<uint8>(type);
    window->mouse_button_status_cache_ ^= static_cast<uint8>(type);
    window->config_.delegate->OnWindowMouseButtonUp(type);
  }
};

} // namespace xEngine

#endif // X_MACOS

#endif // XENGINE_WINDOW_MACOS_MACOSWINDOW_H