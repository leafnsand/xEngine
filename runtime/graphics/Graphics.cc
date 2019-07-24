#include "Graphics.h"

#if X_OPENGL
#include "graphics/impl/OpenGL/OpenGLRenderer.h"
#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResourceManager.h"
#endif
#if X_D3D11
#include "graphics/impl/D3D11/D3D11Renderer.h"
#include "graphics/impl/D3D11/resource/D3D11GraphicsResourceManager.h"
#endif

namespace xEngine {

void Graphics::Initialize(const GraphicsConfig &config) {
  x_assert(!Available());
  config_ = config;
  x_assert(config.window != kInvalidResourceID);
  switch (config_.type) {
    case GraphicsType::kOpenGL3:
#if X_OPENGL
      renderer_.reset(new OpenGLRenderer);
#endif
      break;
    case GraphicsType::kD3D11:
#if X_D3D11
      renderer_.reset(new D3D11Renderer);
#endif
      break;
    default:
      Log::GetInstance().Error("[Graphics::Initialize] unsupported graphics type!\n");
      break;
  }
  x_assert(renderer_);
  renderer_->Initialize(config_);
  switch (config_.type) {
  case GraphicsType::kOpenGL3:
#if X_OPENGL
    resource_manager_.reset(new OpenGLGraphicsResourceManager);
#endif
    break;
  case GraphicsType::kD3D11:
#if X_D3D11
    resource_manager_.reset(new D3D11GraphicsResourceManager(static_cast<D3D11Renderer *>(renderer_.get())->device_));
#endif
    break;
  default:
    Log::GetInstance().Error("[Graphics::Initialize] unsupported graphics type!\n");
    break;
  }
  x_assert(resource_manager_);
  resource_manager_->Initialize(config_);
}
void Graphics::Finalize() {
  x_assert(Available());
  config_ = GraphicsConfig();
  renderer_->MakeCurrent();
  resource_manager_->Finalize();
  renderer_->Finalize();
  resource_manager_.reset();
  renderer_.reset();
}

bool Graphics::Available() const {
  return renderer_ != nullptr;
}

}