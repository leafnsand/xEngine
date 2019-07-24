#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLSHADERFACTORY_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLSHADERFACTORY_H

#if X_OPENGL

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class OpenGLShaderFactory: public ResourceFactory<OpenGLShader> {
 public:
  virtual void Create(OpenGLShader &resource) override;

  virtual void Destroy(OpenGLShader &resource) override;
};

}

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLSHADERFACTORY_H