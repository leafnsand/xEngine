#ifndef XENGINE_WINDOW_WIN32_WIN32WINDOW_H
#define XENGINE_WINDOW_WIN32_WIN32WINDOW_H

#if X_WINDOWS

#include "window/WindowInterface.h"

#include "application/ApplicationDelegate.h"

#include <Windows.h>

namespace xEngine {

class Win32Window : public WindowInterface, ApplicationTickDelegate {
 public:
  virtual void Create(const WindowConfig &config) override;

  virtual void Destroy() override;

  virtual void SetTitle(const eastl::string &name) override;

  virtual void *GetNativeHandle() override { return window_; }

  virtual void OnBeforeEventTick() override;

  virtual void OnAfterEventTick() override;

 private:
  HWND window_{nullptr};

  friend LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

} // namespace xEngine

#endif

#endif // XENGINE_WINDOW_WIN32_WIN32WINDOW_H