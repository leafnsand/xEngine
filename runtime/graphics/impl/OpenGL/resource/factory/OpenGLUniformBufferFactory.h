#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLUNIFORMBUFFERFACTORY_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLUNIFORMBUFFERFACTORY_H

#if X_OPENGL

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class OpenGLUniformBufferFactory: public ResourceFactory<OpenGLUniformBuffer> {
 public:
  virtual void Create(OpenGLUniformBuffer &resource) override;

  virtual void Destroy(OpenGLUniformBuffer &resource) override;
};

} // namespace xEngine

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLUNIFORMBUFFERFACTORY_H