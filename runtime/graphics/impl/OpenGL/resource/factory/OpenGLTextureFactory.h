#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLTEXTUREFACTORY_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLTEXTUREFACTORY_H

#if X_OPENGL

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "resource/ResourceFactory.h"

namespace xEngine {

class OpenGLTextureFactory: public ResourceFactory<OpenGLTexture> {
 public:
  virtual void Create(OpenGLTexture &resource) override;

  virtual void Destroy(OpenGLTexture &resource) override;
};

} // namespace xEngine

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_FACTORY_OPENGLTEXTUREFACTORY_H