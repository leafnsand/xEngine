#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLPIPELINEFACTORY_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLPIPELINEFACTORY_H

#if X_OPENGL

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class OpenGLPipelineFactory: public ResourceFactory<OpenGLPipeline> {
 public:
  virtual void Create(OpenGLPipeline &resource) override;

  virtual void Destroy(OpenGLPipeline &resource) override;
};

} // namespace xEngine

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLPIPELINEFACTORY_H