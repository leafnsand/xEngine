#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLMESHFACTORY_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLMESHFACTORY_H

#if X_OPENGL

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class OpenGLMeshFactory: public ResourceFactory<OpenGLMesh> {
 public:
  virtual void Create(OpenGLMesh &resource) override;

  virtual void Destroy(OpenGLMesh &resource) override;
};

} // namespace xEngine

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLMESHFACTORY_H