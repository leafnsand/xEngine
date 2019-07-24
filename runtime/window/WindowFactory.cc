#include "WindowFactory.h"

#if X_WINDOWS
# include "window/win32/Win32Window.h"
#elif X_MACOS
# include "window/macOS/MacOSWindow.h"
#endif

namespace xEngine {

void WindowFactory::Create(WindowResource &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();
#if X_WINDOWS
  resource.window.reset(new Win32Window);
#elif X_MACOS
  resource.window.reset(new MacOSWindow);
#endif
  if (resource.window != nullptr) {
    resource.window->Create(resource.config());
  }
  resource.window == nullptr ? resource.Failed() : resource.Complete();
}

void WindowFactory::Destroy(WindowResource &resource) {
  x_assert(resource.status() == ResourceStatus::kCompleted || resource.status() == ResourceStatus::kFailed);
  if (resource.window->graphics()->Available()) {
    resource.window->graphics()->Finalize();
  }
  resource.window->Destroy();
}

}
