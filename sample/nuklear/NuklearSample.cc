#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "graphics/Graphics.h"
#include "window/Window.h"
#include "nuklear/NuklearGUI.h"

#define LEN(a) (sizeof(a)/sizeof(a)[0])
#define MAX(a,b) ((a) < (b) ? (b) : (a))
#include <demo/overview.c>
#undef LEN
#undef MAX

using namespace xEngine;

class NuklearSample: public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "NuklearSample"));
    Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
    Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();
    gui_ = NuklearGUI::Create();
    gui_->Initialize(NuklearGUIConfig::ForWindow(window_id_));
  }

  virtual void Finalize() override {
    Window::GetInstance().Finalize();
  }

  virtual void OnWindowClose() override {
    gui_->Finalize();
    gui_.reset();
    Window::GetInstance().GetGraphics(window_id_)->Finalize();
    Window::GetInstance().Destroy(window_id_);
    window_id_ = kInvalidResourceID;
#if X_WINDOWS
    Application::GetInstance().Quit();
#endif
  }

  virtual void OnWindowUpdate() override {
    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyTarget(kInvalidResourceID, ClearState::ClearAll());
    gui_->BeginFrame();
    overview(gui_->context());
    gui_->EndFrame();
    renderer->Render();
  }

 private:
  ResourceID window_id_{kInvalidResourceID};
  NuklearGUIPtr gui_;
};

XENGINE_WINDOW_APPLICATION(NuklearSample)