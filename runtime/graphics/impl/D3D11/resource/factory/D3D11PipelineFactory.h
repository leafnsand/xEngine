#ifndef XENGINE_GRAPHICS_IMPL_D3D11_RESOURCE_FACTORY_D3D11PIPELINEFACTORY_H
#define XENGINE_GRAPHICS_IMPL_D3D11_RESOURCE_FACTORY_D3D11PIPELINEFACTORY_H

#if X_D3D11

#include "graphics/impl/D3D11/resource/D3D11GraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class D3D11GraphicsResourceManager;

class D3D11PipelineFactory: public ResourceFactory<D3D11Pipeline> {
 public:
  D3D11PipelineFactory(D3D11GraphicsResourceManager *manager, ID3D11Device *device) :
    manager_(manager),
    device_(device) {}

  virtual void Create(D3D11Pipeline &resource) override;

  virtual void Destroy(D3D11Pipeline &resource) override;

 private:
  D3D11GraphicsResourceManager *manager_{nullptr};
  ID3D11Device *device_{nullptr};
};

} // namespace xEngine

#endif // X_D3D11

#endif // XENGINE_GRAPHICS_IMPL_D3D11_RESOURCE_FACTORY_D3D11PIPELINEFACTORY_H