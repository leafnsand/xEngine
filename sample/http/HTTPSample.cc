#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "io/IO.h"
#include "http/HTTPFilesystem.h"

using namespace xEngine;

class HTTPSample : public ApplicationDelegate {
 public:
  virtual void Initialize() override {
    Log::GetInstance().Info("Initialize\n");
    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("web", "http://leafnsand.com/");
    IO::GetInstance().RegisterFilesystem("http", HTTPFilesystem::Creator);

    IO::GetInstance().Read("web:index.html", [&](Location location, IOStatus status, DataPtr data) {
      if (status == IOStatus::kSuccess) {
        Log::GetInstance().Debug("read complete: %s\n", data->buffer());

#if X_WINDOWS
        Application::GetInstance().Quit();
#endif
      }
    });
  }

  virtual void Finalize() override {
    IO::GetInstance().Finalize();
  }
};

XENGINE_APPLICATION(HTTPSample)