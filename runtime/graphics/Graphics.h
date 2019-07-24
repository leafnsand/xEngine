#ifndef XENGINE_GRAPHICS_GRAPHICS_H
#define XENGINE_GRAPHICS_GRAPHICS_H

#include "config/GraphicsConfig.h"
#include "RendererInterface.h"
#include "GraphicsResourceManagerInterface.h"

#include <EASTL/unique_ptr.h>

namespace xEngine {

class Graphics {
 public:
  void Initialize(const GraphicsConfig &config);

  void Finalize();

  bool Available() const;

  const GraphicsConfig &config() const { return config_; }

  const eastl::unique_ptr<RendererInterface> &renderer() const { return renderer_; }

  const eastl::unique_ptr<GraphicsResourceManagerInterface> &resource_manager() const { return resource_manager_; }

 private:
  GraphicsConfig config_;
  eastl::unique_ptr<RendererInterface> renderer_{nullptr};
  eastl::unique_ptr<GraphicsResourceManagerInterface> resource_manager_{nullptr};
};

}

#endif // XENGINE_GRAPHICS_GRAPHICS_H