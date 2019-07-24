#include "ShaderCompileToolWindow.h"

#include <application/ApplicationDelegate.h>
#include <io/IO.h>
#include <storage/Storage.h>
#include <window/Window.h>

namespace xEngine {

class ShaderCompileTool : public ApplicationDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("texture", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("texture").string() + Path::separator());
    IO::GetInstance().AddPlaceholder("shader", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("shader").string() + Path::separator());
    IO::GetInstance().RegisterFilesystem("storage", StorageFilesystem::Creator);
    window_ = new ShaderCompileToolWindow;
  }

  virtual void Finalize() override {
    delete window_;
    IO::GetInstance().Finalize();
    Window::GetInstance().Finalize();
  }

 private:
  ShaderCompileToolWindow *window_{nullptr};
};

} // namespace xEngine

XENGINE_WINDOW_APPLICATION(xEngine::ShaderCompileTool)
