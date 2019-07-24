#include "application/ApplicationDelegate.h"
#include "core/Log.h"

using namespace xEngine;

class ApplicationSample : public ApplicationDelegate {
 public:
  virtual void Initialize() override {
    Log::GetInstance().Info("Initialize\n");

#if X_WINDOWS
    Application::GetInstance().Quit();
#endif
  }

  virtual void Finalize() override {
    Log::GetInstance().Info("Finalize\n");
  }
};

XENGINE_APPLICATION(ApplicationSample)