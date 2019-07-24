#include "ShaderCompileToolWindow.h"

#include <application/Application.h>
#include <window/Window.h>

#include <hlslcc.hpp>

#include <D3Dcompiler.h>

namespace xEngine {

static const auto window_width = 800;

static const auto window_height = 600;

ShaderCompileToolWindow::ShaderCompileToolWindow() {
  x_assert(Window::GetInstance().Available());
  nuklear_ = NuklearGUI::CreateUnique();
  window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, window_width, window_height, "ShaderCompileTool"));
  Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
  Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();
  nuklear_->Initialize(NuklearGUIConfig::ForWindow(window_id_));
}

ShaderCompileToolWindow::~ShaderCompileToolWindow() {
  if (window_id_ != kInvalidResourceID) {
    ShaderCompileToolWindow::OnWindowClose();
  }
}

void ShaderCompileToolWindow::OnWindowUpdate() {
  Draw();
}

void ShaderCompileToolWindow::OnWindowClose() {
  nuklear_->Finalize();
  Window::GetInstance().GetGraphics(window_id_)->Finalize();
  Window::GetInstance().Destroy(window_id_);
  window_id_ = kInvalidResourceID;
#if X_WINDOWS
  Application::GetInstance().Quit();
#endif
}

void ShaderCompileToolWindow::OnWindowDropFile(eastl::vector<eastl::string> files) {
  for (auto &file : files) {
    // TODO D3DCompile()
  }
}

void ShaderCompileToolWindow::Draw() {
  auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
  renderer->MakeCurrent();
  renderer->ApplyTarget();
  DrawGUI();
  renderer->Render();
}

void ShaderCompileToolWindow::DrawGUI() {
  auto &window_config = Window::GetInstance().Get(window_id_)->config();
  nuklear_->BeginFrame();
  auto ctx = nuklear_->context();
  nk_begin(ctx, "", nk_rect(-1, -1, window_config.width + 2, window_config.height + 2), NK_WINDOW_BACKGROUND);

  nk_end(ctx);
  nuklear_->EndFrame();
}

} // namespace xEngine
