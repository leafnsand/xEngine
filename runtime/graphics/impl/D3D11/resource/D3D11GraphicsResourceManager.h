#ifndef XENGINE_GRAPHICS_IMPL_D3D11_RESOURCE_D3D11GRAPHICSRESOURCEMANAGER_H
#define XENGINE_GRAPHICS_IMPL_D3D11_RESOURCE_D3D11GRAPHICSRESOURCEMANAGER_H

#if X_D3D11

#include "D3D11GraphicsResource.h"
#include "graphics/GraphicsResourceManagerInterface.h"

#include "factory/D3D11ShaderFactory.h"
#include "factory/D3D11TextureFactory.h"
#include "factory/D3D11MeshFactory.h"
#include "factory/D3D11PipelineFactory.h"
#include "factory/D3D11SamplerFactory.h"
#include "factory/D3D11UniformBufferFactory.h"

#include "resource/ResourcePool.h"

namespace xEngine {

using D3D11ShaderPool = ResourcePool<D3D11Shader, ShaderConfig>;
using D3D11TexturePool = ResourcePool<D3D11Texture, TextureConfig>;
using D3D11MeshPool = ResourcePool<D3D11Mesh, MeshConfig>;
using D3D11PipelinePool = ResourcePool<D3D11Pipeline, PipelineConfig>;
using D3D11SamplerPool = ResourcePool<D3D11Sampler, SamplerConfig>;
using D3D11UniformBufferPool = ResourcePool<D3D11UniformBuffer, UniformBufferConfig>;

class D3D11GraphicsResourceManager: public GraphicsResourceManagerInterface {
 public:
  explicit D3D11GraphicsResourceManager(ID3D11Device *device) :
    shader_factory_(device),
    texture_factory_(device),
    mesh_factory_(device),
    pipeline_factory_(this, device),
    sampler_factory_(device),
    uniform_buffer_factory_(device) {}

  virtual void Initialize(const GraphicsConfig &config) override;

  virtual void Finalize() override;

  virtual ResourceID Create(const ShaderConfig &config) override;

  virtual ResourceID Create(const TextureConfig &config) override;

  virtual ResourceID Create(const MeshConfig &config) override;

  virtual ResourceID Create(const PipelineConfig &config) override;

  virtual ResourceID Create(const SamplerConfig &config) override;

  virtual ResourceID Create(const UniformBufferConfig &config) override;

  virtual void Destroy(ResourceID id) override;

  virtual ShaderInfo QueryInfo(ResourceID id) override;

 private:
  D3D11ShaderPool shader_pool_;
  D3D11TexturePool texture_pool_;
  D3D11MeshPool mesh_pool_;
  D3D11PipelinePool pipeline_pool_;
  D3D11SamplerPool sampler_pool_;
  D3D11UniformBufferPool uniform_buffer_pool_;

  D3D11ShaderFactory shader_factory_;
  D3D11TextureFactory texture_factory_;
  D3D11MeshFactory mesh_factory_;
  D3D11PipelineFactory pipeline_factory_;
  D3D11SamplerFactory sampler_factory_;
  D3D11UniformBufferFactory uniform_buffer_factory_;

  friend class D3D11Renderer;
  friend class D3D11PipelineFactory;
};

} // namespace xEngine

#endif // X_D3D11

#endif // XENGINE_GRAPHICS_IMPL_D3D11_RESOURCE_D3D11GRAPHICSRESOURCEMANAGER_H
