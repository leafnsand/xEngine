#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_OPENGLGRAPHICSRESOURCEMANAGER_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_OPENGLGRAPHICSRESOURCEMANAGER_H

#if X_OPENGL

#include "OpenGLGraphicsResource.h"
#include "graphics/GraphicsResourceManagerInterface.h"

#include "factory/OpenGLShaderFactory.h"
#include "factory/OpenGLTextureFactory.h"
#include "factory/OpenGLMeshFactory.h"
#include "factory/OpenGLPipelineFactory.h"
#include "factory/OpenGLSamplerFactory.h"
#include "factory/OpenGLUniformBufferFactory.h"

#include "resource/ResourcePool.h"

namespace xEngine {

using OpenGLShaderPool = ResourcePool<OpenGLShader, ShaderConfig>;
using OpenGLTexturePool = ResourcePool<OpenGLTexture, TextureConfig>;
using OpenGLMeshPool = ResourcePool<OpenGLMesh, MeshConfig>;
using OpenGLPipelinePool = ResourcePool<OpenGLPipeline, PipelineConfig>;
using OpenGLSamplerPool = ResourcePool<OpenGLSampler, SamplerConfig>;
using OpenGLUniformBufferPool = ResourcePool<OpenGLUniformBuffer, UniformBufferConfig>;

class OpenGLGraphicsResourceManager: public GraphicsResourceManagerInterface {
 public:
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
  OpenGLShaderPool shader_pool_;
  OpenGLTexturePool texture_pool_;
  OpenGLMeshPool mesh_pool_;
  OpenGLPipelinePool pipeline_pool_;
  OpenGLSamplerPool sampler_pool_;
  OpenGLUniformBufferPool uniform_buffer_pool_;

  OpenGLShaderFactory shader_factory_;
  OpenGLTextureFactory texture_factory_;
  OpenGLMeshFactory mesh_factory_;
  OpenGLPipelineFactory pipeline_factory_;
  OpenGLSamplerFactory sampler_factory_;
  OpenGLUniformBufferFactory uniform_buffer_factory_;

  friend class OpenGLRenderer;
};

}

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_OPENGLGRAPHICSRESOURCEMANAGER_H