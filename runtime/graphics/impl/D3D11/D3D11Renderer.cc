#if X_D3D11

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "D3D11Define.h"
#include "D3D11Renderer.h"
#include "graphics/impl/D3D11/resource/D3D11GraphicsResourceManager.h"

#include "window/Window.h"

#include <Windows.h>

namespace xEngine {

void D3D11Renderer::Initialize(const GraphicsConfig &config) {
  config_ = config;

  int32 flags = D3D11_CREATE_DEVICE_SINGLETHREADED;
#if X_DEBUG
  flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  DXGI_SWAP_CHAIN_DESC swap_chain_desc;
  ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
  swap_chain_desc.BufferCount = 1;
  swap_chain_desc.BufferDesc.Width = window()->config().width;
  swap_chain_desc.BufferDesc.Height = window()->config().height;
  swap_chain_desc.BufferDesc.Format = SwapChainFormatForPixelFormat(config_.color_format);
  swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
  swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_desc.OutputWindow = static_cast<HWND>(window()->GetNativeHandle());
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.SampleDesc.Quality = config_.sample_count > 1 ? D3D11_STANDARD_MULTISAMPLE_PATTERN : 0;
  swap_chain_desc.Windowed = !window()->config().is_full_screen;

  D3D_FEATURE_LEVEL feature_level;
  x_d3d11_assert_msg(D3D11CreateDeviceAndSwapChain(
    nullptr,
    D3D_DRIVER_TYPE_HARDWARE,
    nullptr,
    flags,
    nullptr,
    0,
    D3D11_SDK_VERSION,
    &swap_chain_desc,
    &swap_chain_,
    &device_,
    &feature_level,
    &context_
  ), "D3D11 init error!\n");

  x_assert(device_ && context_ && swap_chain_);

  x_d3d11_assert_msg(swap_chain_->GetBuffer(
    0,
    __uuidof(ID3D11Texture2D),
    reinterpret_cast<void **>(&render_target_)
  ), "get render target failed!\n");

  x_assert(render_target_);

  x_d3d11_assert_msg(device_->CreateRenderTargetView(
    render_target_,
    nullptr,
    &render_target_view_
  ), "create render target view failed!\n");

  x_assert(render_target_view_);

  if (config_.depth_format != PixelFormat::NONE) {
    D3D11_TEXTURE2D_DESC depth_stencil_desc;
    ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
    depth_stencil_desc.Width = window()->config().width;
    depth_stencil_desc.Height = window()->config().height;
    depth_stencil_desc.MipLevels = 1;
    depth_stencil_desc.ArraySize = 1;
    depth_stencil_desc.Format = RenderTargetFormatForPixelFormat(config_.depth_format);
    depth_stencil_desc.SampleDesc = swap_chain_desc.SampleDesc;
    depth_stencil_desc.Usage = D3D11_USAGE_DEFAULT;
    depth_stencil_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depth_stencil_desc.CPUAccessFlags = 0;
    depth_stencil_desc.MiscFlags = 0;

    x_d3d11_assert_msg(device_->CreateTexture2D(
      &depth_stencil_desc,
      nullptr,
      &depth_stencil_buffer_
    ), "create depth and stencil buffer failed!\n");

    x_assert(depth_stencil_buffer_);

    D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
    ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));
    depth_stencil_view_desc.Format = depth_stencil_desc.Format;
    depth_stencil_view_desc.ViewDimension = config_.sample_count > 1 ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    depth_stencil_view_desc.Texture2D.MipSlice = 0;

    x_d3d11_assert_msg(device_->CreateDepthStencilView(
      depth_stencil_buffer_,
      &depth_stencil_view_desc,
      &depth_stencil_view_
    ), "create depth and stencil view failed!\n");

    x_assert(depth_stencil_view_);
  }

  Reset();
}

void D3D11Renderer::Finalize() {
  if (render_target_) {
    render_target_->Release();
    render_target_ = nullptr;
  }
  if (render_target_view_) {
    render_target_view_->Release();
    render_target_view_ = nullptr;
  }
  if (depth_stencil_buffer_) {
    depth_stencil_buffer_->Release();
    depth_stencil_buffer_ = nullptr;
  }
  if (depth_stencil_view_) {
    depth_stencil_view_->Release();
    depth_stencil_view_ = nullptr;
  }
  if (swap_chain_) {
    swap_chain_->Release();
    swap_chain_ = nullptr;
  }
  if (context_) {
    context_->Release();
    context_ = nullptr;
  }
#if X_DEBUG
  ID3D11Debug *d3d11_debug;
  x_d3d11_assert_msg(device_->QueryInterface(
    __uuidof(ID3D11Debug),
    reinterpret_cast<void **>(&d3d11_debug)
  ), "get d3d11 debug failed!\n");
  d3d11_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
  if (d3d11_debug != nullptr) {
    d3d11_debug->Release();
  }
#endif
  if (device_) {
    device_->Release();
    device_ = nullptr;
  }
  config_ = GraphicsConfig();
}

void D3D11Renderer::Render() {
  swap_chain_->Present(config_.swap_interval, 0);
}

void D3D11Renderer::MakeCurrent() {}

void D3D11Renderer::ApplyTarget(ResourceID id, const ClearState &state) {
  if (id != kInvalidResourceID) {
    auto &texture = resource_manager()->texture_pool_.Find(id);
    if (texture.status() == ResourceStatus::kCompleted &&
        texture.render_target_view != nullptr &&
        texture.depth_stencil_view != nullptr) {
      if (texture.render_target_view != cache_.render_target_view ||
          texture.depth_stencil_view != cache_.depth_stencil_view) {
        context_->OMSetRenderTargets(1, &texture.render_target_view, texture.depth_stencil_view);
        ApplyViewPort(0, 0, texture.config().width, texture.config().height);
        cache_.render_target_view = texture.render_target_view;
        cache_.depth_stencil_view = texture.depth_stencil_view;
      }
    }
  }
  else {
    if (render_target_view_ != cache_.render_target_view ||
        depth_stencil_view_ != cache_.depth_stencil_view) {
      context_->OMSetRenderTargets(1, &render_target_view_, depth_stencil_view_);
      ApplyViewPort(0, 0, window()->config().frame_buffer_width, window()->config().frame_buffer_height);
      cache_.render_target_view = render_target_view_;
      cache_.depth_stencil_view = depth_stencil_view_;
    }
  }

  if (static_cast<uint8>(state.type) & static_cast<uint8>(ClearType::kColor)) {
    if (cache_.render_target_view != nullptr) {
      context_->ClearRenderTargetView(cache_.render_target_view, glm::value_ptr(state.clear_color));
    }
  }

  auto flags = 0;

  if (static_cast<uint8>(state.type) & static_cast<uint8>(ClearType::kDepth)) {
    flags |= D3D11_CLEAR_DEPTH;
  }

  if (static_cast<uint8>(state.type) & static_cast<uint8>(ClearType::kStencil)) {
    flags |= D3D11_CLEAR_STENCIL;
  }

  if (cache_.depth_stencil_view != nullptr) {
    context_->ClearDepthStencilView(cache_.depth_stencil_view, flags, state.depth, state.stencil);
  }
}

void D3D11Renderer::ApplyViewPort(int32 x, int32 y, int32 width, int32 height) {
  if (cache_.viewport.TopLeftX != x ||
      cache_.viewport.TopLeftY != y ||
      cache_.viewport.Width != width ||
      cache_.viewport.Height != height) {
    cache_.viewport.TopLeftX = static_cast<FLOAT>(x);
    cache_.viewport.TopLeftY = static_cast<FLOAT>(window()->config().frame_buffer_height - y - height);
    cache_.viewport.Width = static_cast<FLOAT>(width);
    cache_.viewport.Height = static_cast<FLOAT>(height);
    cache_.viewport.MinDepth = 0.0f;
    cache_.viewport.MaxDepth = 1.0f;
    context_->RSSetViewports(1, &cache_.viewport);
  }
}

void D3D11Renderer::ApplyScissor(int32 x, int32 y, int32 width, int32 height) {
  if (cache_.scissor.left != x ||
      cache_.scissor.top != y ||
      cache_.scissor.right != x + width ||
      cache_.scissor.bottom != y + height) {
    cache_.scissor.left = static_cast<FLOAT>(x);
    cache_.scissor.top = static_cast<FLOAT>(window()->config().frame_buffer_height - y - height);
    cache_.scissor.right = static_cast<FLOAT>(x + width);
    cache_.scissor.bottom = static_cast<FLOAT>(window()->config().frame_buffer_height - y);
    context_->RSSetScissorRects(1, &cache_.scissor);
  }
}

void D3D11Renderer::ApplyPipeline(ResourceID id) {
  auto &pipeline = resource_manager()->pipeline_pool_.Find(id);
  if (pipeline.status() == ResourceStatus::kCompleted) {
    ApplyShader(pipeline.config().shader);
    if (pipeline.input_layout != cache_.input_layout) {
      context_->IASetInputLayout(pipeline.input_layout);
      cache_.input_layout = pipeline.input_layout;
    }
    if (pipeline.blend_state != cache_.blend_state) {
      context_->OMSetBlendState(pipeline.blend_state, glm::value_ptr(pipeline.config().blend_state.blend_color), 0xFFFFFFFF);
      cache_.blend_state = pipeline.blend_state;
    }
    if (pipeline.depth_stencil_state != cache_.depth_stencil_state) {
      context_->OMSetDepthStencilState(pipeline.depth_stencil_state, pipeline.config().depth_stencil_state.stencil_value);
      cache_.depth_stencil_state = pipeline.depth_stencil_state;
    }
    if (pipeline.rasterizer_state != cache_.rasterizer_state) {
      context_->RSSetState(pipeline.rasterizer_state);
      cache_.rasterizer_state = pipeline.rasterizer_state;
    }
  }
}

void D3D11Renderer::ResetPipeline() {
  context_->IASetInputLayout(nullptr);
  cache_.input_layout = nullptr;
  context_->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
  cache_.blend_state = nullptr;
  context_->OMSetDepthStencilState(nullptr, 0xFFFFFFFF);
  cache_.depth_stencil_state = nullptr;
  context_->RSSetState(nullptr);
  cache_.rasterizer_state = nullptr;
}

void D3D11Renderer::ApplyShader(ResourceID id) {
  auto &shader = resource_manager()->shader_pool_.Find(id);
  if (shader.status() == ResourceStatus::kCompleted) {
    ApplyD3D11Shader(shader);
  }
}

void D3D11Renderer::UpdateShaderResourceData(ResourceID shader_id, const eastl::string &name, DataPtr data) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    ApplyD3D11Shader(shader);

    auto find = false;

    if (shader.vertex_global_uniform_block != nullptr) {
      const auto &element_pair = shader.info.vertex.global_uniform_block_info.elements.find(name.c_str());
      if (element_pair != shader.info.vertex.global_uniform_block_info.elements.end()) {
        find = true;
        const auto &info = element_pair->second;
        if (data != nullptr && data->size() >= info.size) {
          memcpy(static_cast<uint8 *>(shader.vertex_global_uniform_buffer) + info.offset, data->buffer(), info.size);
          D3D11_MAPPED_SUBRESOURCE source;
          ZeroMemory(&source, sizeof(source));
          x_d3d11_assert_msg(context_->Map(shader.vertex_global_uniform_block, 0, D3D11_MAP_WRITE_DISCARD, 0, &source), "map vertex global uniform buffer falied!\n");
          memcpy(source.pData, shader.vertex_global_uniform_buffer, shader.info.vertex.global_uniform_block_info.size);
          context_->Unmap(shader.vertex_global_uniform_block, 0);
        } else {
          Log::GetInstance().Error("buffer size is too small to fit into uniform: %s\n", name.c_str());
        }
      }
    }

    if (shader.fragment_global_uniform_block != nullptr) {
      const auto &element_pair = shader.info.fragment.global_uniform_block_info.elements.find(name.c_str());
      if (element_pair != shader.info.fragment.global_uniform_block_info.elements.end()) {
        find = true;
        const auto &info = element_pair->second;
        if (data != nullptr && data->size() >= info.size) {
          memcpy(static_cast<uint8 *>(shader.fragment_global_uniform_buffer) + info.offset, data->buffer(), info.size);
          D3D11_MAPPED_SUBRESOURCE source;
          ZeroMemory(&source, sizeof(source));
          x_d3d11_assert_msg(context_->Map(shader.fragment_global_uniform_block, 0, D3D11_MAP_WRITE_DISCARD, 0, &source), "map fragment global uniform buffer falied!\n");
          memcpy(source.pData, shader.fragment_global_uniform_buffer, shader.info.fragment.global_uniform_block_info.size);
          context_->Unmap(shader.fragment_global_uniform_block, 0);
        } else {
          Log::GetInstance().Error("buffer size is too small to fit into uniform: %s\n", name.c_str());
        }
      }
    }

    if (!find) {
      Log::GetInstance().Error("cannot find uniform: %s\n", name.c_str());
    }
  }
}

void D3D11Renderer::UpdateShaderResourceTexture(ResourceID shader_id, const eastl::string &name, ResourceID texture_id) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    ApplyD3D11Shader(shader);

    auto find = false;

    const auto &vertex_pair = shader.info.vertex.texture_index.find(name.c_str());
    if (vertex_pair != shader.info.vertex.texture_index.end()) {
      ApplyTexture(texture_id, vertex_pair->second, GraphicsPipelineStage::kVertexShader);
      find = true;
    }

    const auto &fragment_pair = shader.info.fragment.texture_index.find(name.c_str());
    if (fragment_pair != shader.info.fragment.texture_index.end()) {
      ApplyTexture(texture_id, fragment_pair->second, GraphicsPipelineStage::kFragmentShader);
      find = true;
    }

    if (!find) {
      Log::GetInstance().Error("cannot find uniform texture: %s\n", name.c_str());
    }
  }
}

void D3D11Renderer::UpdateShaderResourceSampler(ResourceID shader_id, const eastl::string &name, ResourceID sampler_id) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    ApplyD3D11Shader(shader);

    auto vertex_index = -1;
    auto fragment_index = -1;

    auto vertex_pair = shader.info.vertex.sampler_index.find(name.c_str());
    if (vertex_pair != shader.info.vertex.sampler_index.end()) {
      vertex_index = vertex_pair->second;
    }

    auto fragment_pair = shader.info.fragment.sampler_index.find(name.c_str());
    if (fragment_pair != shader.info.fragment.sampler_index.end()) {
      fragment_index = fragment_pair->second;
    }

    if (vertex_index != -1) {
      ApplySampler(sampler_id, vertex_index, GraphicsPipelineStage::kVertexShader);
    }
    if (fragment_index != -1) {
      ApplySampler(sampler_id, fragment_index, GraphicsPipelineStage::kFragmentShader);
    }

    if (vertex_index == -1 && fragment_index == -1) {
      Log::GetInstance().Error("cannot find uniform sampler state: %s\n", name.c_str());
    }
  }
}

void D3D11Renderer::UpdateShaderResourceBlock(ResourceID shader_id, const eastl::string &name, ResourceID uniform_buffer_id) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    ApplyD3D11Shader(shader);

    auto &uniform_buffer = resource_manager()->uniform_buffer_pool_.Find(uniform_buffer_id);
    if (uniform_buffer.status() == ResourceStatus::kCompleted) {
      const auto &vertex_pair = shader.info.vertex.uniform_block_info.find(name.c_str());
      if (vertex_pair != shader.info.vertex.uniform_block_info.end()) {
        const auto &info = vertex_pair->second;
        if (info.size < uniform_buffer.config().size) {
          Log::GetInstance().Error("bind uniform buffer, but size too large\n");
        } else {
          context_->VSSetConstantBuffers(info.location, 1, &uniform_buffer.uniform_buffer);
        }
      }
      const auto &fragment_pair = shader.info.fragment.uniform_block_info.find(name.c_str());
      if (fragment_pair != shader.info.fragment.uniform_block_info.end()) {
        const auto &info = vertex_pair->second;
        if (info.size < uniform_buffer.config().size) {
          Log::GetInstance().Error("bind uniform buffer, but size too large\n");
        } else {
          context_->PSSetConstantBuffers(info.location, 1, &uniform_buffer.uniform_buffer);
        }
      }
    }
  }
}

void D3D11Renderer::ResetShader() {
  context_->VSSetShader(nullptr, nullptr, 0);
  cache_.vertex_shader = nullptr;
  context_->PSSetShader(nullptr, nullptr, 0);
  cache_.fragment_shader = nullptr;
}

void D3D11Renderer::UpdateUniformBufferData(ResourceID id, DataPtr data) {
  auto &uniform_buffer = resource_manager()->uniform_buffer_pool_.Find(id);
  if (uniform_buffer.status() == ResourceStatus::kCompleted) {
    if (data == nullptr || uniform_buffer.config().size < data->size()) {
      Log::GetInstance().Error("uniform buffer size is smaller then offset + length\n");
    } else {
      if (uniform_buffer.config().size < data->size()) {
        Log::GetInstance().Error("update uniform buffer data, but size < offset + length\n");
      } else {
        D3D11_MAPPED_SUBRESOURCE uniform_buffer_source;
        ZeroMemory(&uniform_buffer_source, sizeof(uniform_buffer_source));
        x_d3d11_assert_msg(context_->Map(uniform_buffer.uniform_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &uniform_buffer_source), "map uniform buffer falied!\n");
        memcpy(uniform_buffer_source.pData, data->buffer(), data->size());
        context_->Unmap(uniform_buffer.uniform_buffer, 0);
      }
    }
  }
}

void D3D11Renderer::ApplyMesh(ResourceID id) {
  auto &mesh = resource_manager()->mesh_pool_.Find(id);
  if (mesh.status() == ResourceStatus::kCompleted) {
    // TODO multi vertex buffer
    if (mesh.vertex_buffer != cache_.vertex_buffer) {
      static const uint32 offset = 0;
      context_->IASetVertexBuffers(0, 1, &mesh.vertex_buffer, reinterpret_cast<UINT *>(&mesh.config().layout.size), &offset);
      cache_.vertex_buffer = mesh.vertex_buffer;
    }
    if (mesh.index_buffer != cache_.index_buffer) {
      context_->IASetIndexBuffer(mesh.index_buffer, EnumForIndexFormat(mesh.config().index_type), 0);
      cache_.index_buffer = mesh.index_buffer;
      cache_.index_format = mesh.config().index_type;
    }
  }
}

void D3D11Renderer::UpdateMesh(ResourceID id, DataPtr vertex_data, DataPtr index_data) {
  auto &mesh = resource_manager()->mesh_pool_.Find(id);
  if (mesh.status() == ResourceStatus::kCompleted) {
    // TODO multi vertex buffer
    if (mesh.vertex_buffer != cache_.vertex_buffer) {
      static const uint32 offset = 0;
      context_->IASetVertexBuffers(0, 1, &mesh.vertex_buffer, reinterpret_cast<UINT *>(&mesh.config().layout.size), &offset);
      cache_.vertex_buffer = mesh.vertex_buffer;
    }
    if (vertex_data != nullptr && vertex_data->size() > 0) {
      if (mesh.config().layout.size * mesh.config().vertex_count < vertex_data->size()) {
        Log::GetInstance().Error("update vertex buffer data, but size < offset + length\n");
      } else {
        D3D11_MAPPED_SUBRESOURCE vertex_source;
        ZeroMemory(&vertex_source, sizeof(vertex_source));
        x_d3d11_assert_msg(context_->Map(mesh.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertex_source), "map vertex buffer falied!\n");
        memcpy(vertex_source.pData, vertex_data->buffer(), vertex_data->size());
        context_->Unmap(mesh.vertex_buffer, 0);
      }
    }
    if (mesh.index_buffer != cache_.index_buffer) {
      context_->IASetIndexBuffer(mesh.index_buffer, EnumForIndexFormat(mesh.config().index_type), 0);
      cache_.index_buffer = mesh.index_buffer;
      cache_.index_format = mesh.config().index_type;
    }
    if (index_data != nullptr && index_data->size() > 0) {
      if (mesh.config().index_count * SizeOfIndexFormat(mesh.config().index_type) < index_data->size()) {
        Log::GetInstance().Error("update index buffer data, but size < offset + length\n");
      } else {
        D3D11_MAPPED_SUBRESOURCE index_source;
        ZeroMemory(&index_source, sizeof(index_source));
        x_d3d11_assert_msg(context_->Map(mesh.index_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &index_source), "map index buffer falied!\n");
        memcpy(index_source.pData, index_data->buffer(), index_data->size());
        context_->Unmap(mesh.index_buffer, 0);
      }
    }
  }
}

void D3D11Renderer::ResetMesh() {
  static const uint32 size = 0;
  static const uint32 offset = 0;
  cache_.vertex_buffer = nullptr;
  context_->IASetVertexBuffers(0, 1, &cache_.vertex_buffer, &size, &offset);
  cache_.index_buffer = nullptr;
  cache_.index_format = IndexFormat::kNone;
  context_->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
}

void D3D11Renderer::Draw(const DrawCallState &state) {
  context_->IASetPrimitiveTopology(EnumForVertexTopology(state.topology));
  if (cache_.index_format == IndexFormat::kNone) {
    context_->Draw(state.count, state.first);
  } else {
    context_->DrawIndexed(state.count, state.first, 0);
  }
}

void D3D11Renderer::Reset() {
  ResetShader();
  ResetTexture();
  ResetSampler();
  ResetMesh();
  ResetPipeline();
}

void D3D11Renderer::ApplyD3D11Shader(D3D11Shader &shader) {
  if (shader.vertex_shader != cache_.vertex_shader) {
    context_->VSSetShader(shader.vertex_shader, nullptr, 0);
    if (shader.vertex_global_uniform_block != nullptr) {
      context_->VSSetConstantBuffers(shader.info.vertex.global_uniform_block_info.location, 1, &shader.vertex_global_uniform_block);
    }
    cache_.vertex_shader = shader.vertex_shader;
  }
  if (shader.fragment_shader != cache_.fragment_shader) {
    context_->PSSetShader(shader.fragment_shader, nullptr, 0);
    if (shader.fragment_global_uniform_block != nullptr) {
      context_->PSSetConstantBuffers(shader.info.fragment.global_uniform_block_info.location, 1, &shader.fragment_global_uniform_block);
    }
    cache_.fragment_shader = shader.fragment_shader;
  }
}

void D3D11Renderer::ApplySampler(ResourceID id, int32 index, GraphicsPipelineStage stage) {
  auto &sampler = resource_manager()->sampler_pool_.Find(id);
  if (sampler.status() == ResourceStatus::kCompleted) {
    if (stage == GraphicsPipelineStage::kVertexShader) {
      if (sampler.sampler_state != cache_.vertex_sampler_state[index]) {
        context_->PSSetSamplers(index, 1, &sampler.sampler_state);
        cache_.vertex_sampler_state[index] = sampler.sampler_state;
      }
    }
    else if (stage == GraphicsPipelineStage::kFragmentShader) {
      if (sampler.sampler_state != cache_.fragment_sampler_state[index]) {
        context_->PSSetSamplers(index, 1, &sampler.sampler_state);
        cache_.fragment_sampler_state[index] = sampler.sampler_state;
      }
    }
  }
}

void D3D11Renderer::ResetSampler() {
  memset(cache_.vertex_sampler_state, 0, sizeof(cache_.vertex_sampler_state));
  context_->PSSetSamplers(0, static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount), cache_.vertex_sampler_state);
  memset(cache_.fragment_sampler_state, 0, sizeof(cache_.fragment_sampler_state));
  context_->PSSetSamplers(0, static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount), cache_.fragment_sampler_state);
}

void D3D11Renderer::ApplyTexture(ResourceID id, int32 index, GraphicsPipelineStage stage) {
  auto &texture = resource_manager()->texture_pool_.Find(id);
  if (texture.status() == ResourceStatus::kCompleted) {
    if (stage == GraphicsPipelineStage::kVertexShader) {
      if (texture.shader_resource_view != cache_.vertex_shader_resource_view[index]) {
        context_->PSSetShaderResources(index, 1, &texture.shader_resource_view);
        cache_.vertex_shader_resource_view[index] = texture.shader_resource_view;
      }
    } else if (stage == GraphicsPipelineStage::kFragmentShader) {
      if (texture.shader_resource_view != cache_.fragment_shader_resource_view[index]) {
        context_->PSSetShaderResources(index, 1, &texture.shader_resource_view);
        cache_.fragment_shader_resource_view[index] = texture.shader_resource_view;
      }
    }
  }
}

void D3D11Renderer::ResetTexture() {
  memset(cache_.vertex_shader_resource_view, 0, sizeof(cache_.vertex_shader_resource_view));
  context_->PSSetShaderResources(0, static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount), cache_.vertex_shader_resource_view);
  memset(cache_.fragment_shader_resource_view, 0, sizeof(cache_.fragment_shader_resource_view));
  context_->PSSetShaderResources(0, static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount), cache_.fragment_shader_resource_view);
}

}

#endif