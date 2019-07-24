#if X_D3D11

#include "D3D11PipelineFactory.h"
#include "graphics/impl/D3D11/D3D11Define.h"
#include "graphics/impl/D3D11/resource/D3D11GraphicsResourceManager.h"

#include "core/Log.h"

namespace xEngine {

void D3D11PipelineFactory::Create(D3D11Pipeline &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  auto &shader = manager_->shader_pool_.Find(resource.config().shader);
  if (shader.status() != ResourceStatus::kCompleted || shader.config().vertex == nullptr) {
    Log::GetInstance().Error("[D3D11PipelineFactory::Create] invalid shader\n");
    resource.Failed();
    return;
  }

  auto &layout = resource.config().vertex_layout;
  if (layout.element_count == 0) {
    Log::GetInstance().Error("[D3D11PipelineFactory::Create] input layout is empty\n");
    resource.Failed();
    return;
  }

  ID3D11InputLayout *input_layout = nullptr;

  D3D11_INPUT_ELEMENT_DESC input_element_desc[static_cast<uint16>(GraphicsMaxDefine::kMaxVertexElementCount)];
  for (auto index = 0; index < layout.element_count; ++index) {
    auto &element = layout.elements[index];
    auto &desc = input_element_desc[index];
    ZeroMemory(&desc, sizeof(desc));
    desc.SemanticName = SemanticForVertexElementSemantic(element.semantic);
    desc.SemanticIndex = IndexForVertexElementSemantic(element.semantic);
    desc.Format = EnumForVertexElementFormat(element.format);
    desc.InputSlot = 0; //TODO
    desc.AlignedByteOffset = element.offset;
    desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; // TODO
    desc.InstanceDataStepRate = 0; //TODO
  }

  x_d3d11_assert_msg(device_->CreateInputLayout(
    input_element_desc,
    layout.element_count,
    shader.config().vertex->buffer(),
    shader.config().vertex->size(),
    &input_layout
  ), "create input layout failed\n");

  resource.input_layout = input_layout;

  auto &blend_state_config = resource.config().blend_state;
  ID3D11BlendState *blend_state = nullptr;
  
  D3D11_BLEND_DESC blend_desc;
  ZeroMemory(&blend_desc, sizeof(blend_desc));
  blend_desc.AlphaToCoverageEnable = false;
  blend_desc.IndependentBlendEnable = false;
  blend_desc.RenderTarget[0].BlendEnable = blend_state_config.enable;
  blend_desc.RenderTarget[0].SrcBlend = EnumForBlendFactor(blend_state_config.src_rgb_factor);
  blend_desc.RenderTarget[0].DestBlend = EnumForBlendFactor(blend_state_config.dst_rgb_factor);
  blend_desc.RenderTarget[0].BlendOp = EnumForBlendOperation(blend_state_config.rgb_operation);
  blend_desc.RenderTarget[0].SrcBlendAlpha = EnumForBlendFactor(blend_state_config.src_alpha_factor);
  blend_desc.RenderTarget[0].DestBlendAlpha = EnumForBlendFactor(blend_state_config.dst_alpha_factor);
  blend_desc.RenderTarget[0].BlendOpAlpha = EnumForBlendOperation(blend_state_config.alpha_operation);
  blend_desc.RenderTarget[0].RenderTargetWriteMask = ColorWriteMaskForPixelChannel(blend_state_config.color_mask);

  x_d3d11_assert_msg(device_->CreateBlendState(&blend_desc, &blend_state), "create blend state failed\n");

  resource.blend_state = blend_state;

  auto &depth_stencil_config = resource.config().depth_stencil_state;
  ID3D11DepthStencilState *depth_stencil_state = nullptr;

  D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
  ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
  depth_stencil_desc.DepthEnable = depth_stencil_config.depth_enable;
  depth_stencil_desc.DepthWriteMask = depth_stencil_config.depth_write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
  depth_stencil_desc.DepthFunc = EnumForCompareFunction(depth_stencil_config.depth_compare);
  depth_stencil_desc.StencilEnable = depth_stencil_config.stencil_enable;
  depth_stencil_desc.StencilReadMask = depth_stencil_config.stencil_read_mask;
  depth_stencil_desc.StencilWriteMask = depth_stencil_config.stencil_write_mask;
  depth_stencil_desc.FrontFace.StencilFailOp = EnumForStencilOperation(depth_stencil_config.front.fail);
  depth_stencil_desc.FrontFace.StencilDepthFailOp = EnumForStencilOperation(depth_stencil_config.front.depth_fail);
  depth_stencil_desc.FrontFace.StencilPassOp = EnumForStencilOperation(depth_stencil_config.front.pass);
  depth_stencil_desc.FrontFace.StencilFunc = EnumForCompareFunction(depth_stencil_config.front.compare);
  depth_stencil_desc.BackFace.StencilFailOp = EnumForStencilOperation(depth_stencil_config.back.fail);
  depth_stencil_desc.BackFace.StencilDepthFailOp = EnumForStencilOperation(depth_stencil_config.back.depth_fail);
  depth_stencil_desc.BackFace.StencilPassOp = EnumForStencilOperation(depth_stencil_config.back.pass);
  depth_stencil_desc.BackFace.StencilFunc = EnumForCompareFunction(depth_stencil_config.back.compare);

  x_d3d11_assert_msg(device_->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state), "create depth stencil state failed\n");

  resource.depth_stencil_state = depth_stencil_state;

  auto &rasterizer_config = resource.config().rasterizer_state;
  ID3D11RasterizerState *rasterizer_state = nullptr;

  D3D11_RASTERIZER_DESC rasterizer_desc;
  ZeroMemory(&rasterizer_desc, sizeof(rasterizer_desc));
  rasterizer_desc.FillMode = rasterizer_config.fill_mode == FillMode::kWireframe ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
  rasterizer_desc.CullMode = rasterizer_config.cull_face_enable ? (rasterizer_config.cull_face == FaceSide::kFront ? D3D11_CULL_FRONT : D3D11_CULL_BACK) : D3D11_CULL_NONE;
  rasterizer_desc.FrontCounterClockwise = rasterizer_config.front_face == FrontFaceType::kCounterClockWise;
  rasterizer_desc.DepthBias = 0;
  rasterizer_desc.DepthBiasClamp = 0.0f;
  rasterizer_desc.DepthClipEnable = true;
  rasterizer_desc.ScissorEnable = rasterizer_config.scissor_test_enable;
  rasterizer_desc.MultisampleEnable = rasterizer_config.sample > 1;
  rasterizer_desc.AntialiasedLineEnable = false;

  x_d3d11_assert_msg(device_->CreateRasterizerState(&rasterizer_desc, &rasterizer_state), "create rasterizer state failed\n");

  resource.rasterizer_state = rasterizer_state;

  resource.Complete();
}

void D3D11PipelineFactory::Destroy(D3D11Pipeline &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.input_layout != nullptr) {
    resource.input_layout->Release();
  }
  if (resource.blend_state != nullptr) {
    resource.blend_state->Release();
  }
  if (resource.depth_stencil_state != nullptr) {
    resource.depth_stencil_state->Release();
  }
  if (resource.rasterizer_state != nullptr) {
    resource.rasterizer_state->Release();
  }
}

} // namespace xEngine

#endif // X_D3D11