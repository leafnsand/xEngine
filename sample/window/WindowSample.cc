#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "window/Window.h"

using namespace xEngine;

class WindowSample : public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "WindowSample"));
  }

  virtual void Finalize() override {
    Window::GetInstance().Finalize();
  }

  virtual void OnWindowClose() override {
    Window::GetInstance().Destroy(window_id_);
    window_id_ = kInvalidResourceID;
#if X_WINDOWS
    Application::GetInstance().Quit();
#endif
  }

 private:
  ResourceID window_id_{kInvalidResourceID};
};

XENGINE_WINDOW_APPLICATION(WindowSample)