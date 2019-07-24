#include "application/Application.h"
#include "application/ApplicationDelegate.h"

#include "core/Core.h"

namespace xEngine {

extern void NativeApplicationInitialize(ApplicationDelegate *delegate);

extern void NativeApplicationFinalize(ApplicationDelegate *delegate);

extern bool PollNativeApplicationEvent();

void Application::Run(ApplicationDelegate *delegate) {
  x_assert(delegate_ == nullptr && delegate != nullptr);
  delegate_ = delegate;
  NativeApplicationInitialize(delegate_);
  while (true) {
    InvokeBeforeEvent();
    if (PollNativeApplicationEvent()) break;
    InvokeAfterEvent();
  }
}

void Application::Quit() {
  NativeApplicationFinalize(delegate_);
}

void Application::AddLoopDelegate(ApplicationTickDelegate *delegate) {
  if (delegate != nullptr) {
    loop_delegate_list_.push_back(delegate);
  }
}

void Application::RemoveLoopDelegate(ApplicationTickDelegate *delegate) {
  if (delegate != nullptr) {
    auto iterator = eastl::find(loop_delegate_list_.begin(), loop_delegate_list_.end(), delegate);
    if (iterator != loop_delegate_list_.end()) {
      loop_delegate_list_.erase(iterator);
    }
  }
}

void Application::InvokeBeforeEvent() {
  auto list = loop_delegate_list_;
  for (auto &delegate : list) {
    delegate->OnBeforeEventTick();
  }
}

void Application::InvokeAfterEvent() {
  auto list = loop_delegate_list_;
  for (auto &delegate : list) {
    delegate->OnAfterEventTick();
  }
}

} // namespace xEngine