#ifndef XENGINE_WINDOW_WINDOWFACTORY_H
#define XENGINE_WINDOW_WINDOWFACTORY_H

#include "WindowInterface.h"

#include "resource/ResourceFactory.h"
#include "resource/ResourceIdentity.h"

#include <EASTL/unique_ptr.h>

namespace xEngine {

struct WindowResource: public Resource<WindowConfig> {

  eastl::unique_ptr<WindowInterface> window{nullptr};

  virtual void Reset() override {
    window.reset();
  }

};

class WindowFactory: public ResourceFactory<WindowResource> {
 public:
  virtual void Create(WindowResource &resource) override;

  virtual void Destroy(WindowResource &resource) override;
};

}

#endif // XENGINE_WINDOW_WINDOWFACTORY_H