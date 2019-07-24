#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLSAMPLERFACTORY_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLSAMPLERFACTORY_H

#if X_OPENGL

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class OpenGLSamplerFactory: public ResourceFactory<OpenGLSampler> {
 public:
  virtual void Create(OpenGLSampler &resource) override;

  virtual void Destroy(OpenGLSampler &resource) override;
};

} // namespace xEngine

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLSAMPLERFACTORY_H