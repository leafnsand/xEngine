#ifndef XENGINE_APPLICATION_APPLICATION_H
#define XENGINE_APPLICATION_APPLICATION_H

#include "core/Types.h"

#include <EASTL/list.h>

namespace xEngine {

class ApplicationDelegate;

class ApplicationTickDelegate;

class Application {
 public:
  static Application &GetInstance() {
    static Application instance;
    return instance;
  }

  void Run(ApplicationDelegate *delegate);

  void Quit();

  void AddLoopDelegate(ApplicationTickDelegate *delegate);

  void RemoveLoopDelegate(ApplicationTickDelegate *delegate);

  ApplicationDelegate *delegate() const { return delegate_; }

 private:
  void InvokeBeforeEvent();

  void InvokeAfterEvent();

 private:
  ApplicationDelegate *delegate_{nullptr};
  eastl::list<ApplicationTickDelegate *> loop_delegate_list_;
};

} // namespace xEngine

#endif // XENGINE_APPLICATION_APPLICATION_H