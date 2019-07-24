#ifndef XENGINE_GRAPHICS_IMPL_D3D11_D3D11RENDERER_H
#define XENGINE_GRAPHICS_IMPL_D3D11_D3D11RENDERER_H

#if X_D3D11

#include "graphics/RendererInterface.h"
#include "graphics/impl/D3D11/D3D11Define.h"
#include "graphics/impl/D3D11/resource/D3D11GraphicsResourceManager.h"

#include "window/Window.h"

namespace xEngine {

class WindowInterface;

class D3D11Renderer: public RendererInterface {
 public:
  virtual void Initialize(const GraphicsConfig &config) override;

  virtual void Finalize() override;

  virtual void Render() override;

  virtual void MakeCurrent() override;

  virtual void ApplyTarget(ResourceID id = kInvalidResourceID, const ClearState &state = ClearState::ClearAll()) override;

  virtual void ApplyViewPort(int32 x, int32 y, int32 width, int32 height) override;

  virtual void ApplyScissor(int32 x, int32 y, int32 width, int32 height) override;

  virtual void ApplyPipeline(ResourceID id) override;

  virtual void ResetPipeline() override;

  virtual void ApplyShader(ResourceID id) override;

  virtual void UpdateShaderResourceData(ResourceID shader_id, const eastl::string &name, DataPtr data) override;

  virtual void UpdateShaderResourceTexture(ResourceID shader_id, const eastl::string &name, ResourceID texture_id) override;

  virtual void UpdateShaderResourceSampler(ResourceID shader_id, const eastl::string &name, ResourceID sampler_id) override;

  virtual void UpdateShaderResourceBlock(ResourceID shader_id, const eastl::string &name, ResourceID uniform_buffer_id) override;

  virtual void ResetShader() override;

  virtual void UpdateUniformBufferData(ResourceID id, DataPtr data) override;

  virtual void ApplyMesh(ResourceID id) override;

  virtual void UpdateMesh(ResourceID id, DataPtr vertex_data, DataPtr index_data) override;

  virtual void ResetMesh() override;

  virtual void Draw(const DrawCallState &state) override;

  virtual void Reset() override;

 private:

  void ApplyD3D11Shader(D3D11Shader &shader);

  void ApplySampler(ResourceID id, int32 index, GraphicsPipelineStage stage);

  void ResetSampler();

  void ApplyTexture(ResourceID id, int32 index, GraphicsPipelineStage stage);

  void ResetTexture();

  WindowInterface *window() {
    if (window_ == nullptr) {
      window_ = Window::GetInstance().Get(config_.window).get();
    }
    return window_;
  }

  D3D11GraphicsResourceManager *resource_manager() {
    if (resource_manager_ == nullptr) {
      resource_manager_ = static_cast<D3D11GraphicsResourceManager *>(window()->graphics()->resource_manager().get());
    }
    return resource_manager_;
  }

private:
  GraphicsConfig config_;

  WindowInterface *window_{nullptr};
  D3D11GraphicsResourceManager *resource_manager_{nullptr};

  ID3D11Device *device_{nullptr};
  ID3D11DeviceContext *context_{nullptr};
  IDXGISwapChain *swap_chain_{nullptr};

  ID3D11Texture2D *render_target_{nullptr};
  ID3D11RenderTargetView *render_target_view_{nullptr};

  ID3D11Texture2D *depth_stencil_buffer_{nullptr};
  ID3D11DepthStencilView *depth_stencil_view_{nullptr};

  D3DRendererCache cache_;

  friend class Graphics;
};

} // namespace xEngine

#endif // X_D3D11

#endif // XENGINE_GRAPHICS_IMPL_D3D11_D3D11RENDERER_H
