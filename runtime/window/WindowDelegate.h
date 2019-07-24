#ifndef XENGINE_WINDOW_WINDOWDELEGATE_H
#define XENGINE_WINDOW_WINDOWDELEGATE_H

#include <glm.hpp>

#include <EASTL/string.h>
#include <EASTL/vector.h>

namespace xEngine {

class WindowDelegate {
 public:
  virtual ~WindowDelegate() {}

  virtual void OnWindowUpdate() {}

  virtual void OnWindowClose() {}

  virtual void OnWindowResize() {}

  virtual void OnWindowMousePosition(const glm::vec2 &position) {}

  virtual void OnWindowMouseScroll(const glm::vec2 &offset) {}

  virtual void OnWindowMouseButtonDown(MouseButtonType type) {}

  virtual void OnWindowMouseButtonUp(MouseButtonType type) {}

  virtual void OnWindowKeyboardButtonDown(KeyboardButtonType type) {}

  virtual void OnWindowKeyboardButtonUp(KeyboardButtonType type) {}

  virtual void OnWindowDropFile(eastl::vector<eastl::string> files) {}
};

}

#endif // XENGINE_WINDOW_WINDOWDELEGATE_H