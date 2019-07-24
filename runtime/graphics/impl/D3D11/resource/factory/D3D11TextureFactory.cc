#if X_D3D11

#include "D3D11TextureFactory.h"

#include "graphics/impl/D3D11/D3D11Define.h"

#include "core/Log.h"

namespace xEngine {

void D3D11TextureFactory::Create(D3D11Texture &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  auto &config = resource.config();
  auto face_count = config.type == TextureType::kTextureCube ? static_cast<uint16>(GraphicsMaxDefine::kMaxCubeTextureFaceCount) : 1;

  for (auto i = 0; i < face_count; ++i) {
    for (auto j = 0; j < config.mipmap_count; ++j) {
      auto width = config.width >> j;
      auto height = config.height >> j;
      if (width == 0) width = 1;
      if (height == 0) height = 1;
      if (config.data[i][j] == nullptr ||
        config.data[i][j]->size() < SizeOfPixelFormat(config.color_format) * width * height) {
        Log::GetInstance().Error("[D3D11TextureFactory::Create] wrong data size\n");
        resource.Failed();
        return;
      }
    }
  }

  ID3D11Texture2D *texture = nullptr;

  D3D11_TEXTURE2D_DESC texture_desc;
  ZeroMemory(&texture_desc, sizeof(texture_desc));
  texture_desc.Width = config.width;
  texture_desc.Height = config.height;
  texture_desc.MipLevels = config.mipmap_count;
  texture_desc.ArraySize = face_count;
  texture_desc.Format = TextureFormatForPixelFormat(config.color_format);
  texture_desc.SampleDesc.Count = 1;
  texture_desc.SampleDesc.Quality = 0;
  texture_desc.Usage = D3D11_USAGE_IMMUTABLE;
  texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  texture_desc.CPUAccessFlags = 0;
  texture_desc.MiscFlags = config.type == TextureType::kTextureCube ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;

  auto data_count = 0;
  D3D11_SUBRESOURCE_DATA data[static_cast<uint16>(GraphicsMaxDefine::kMaxCubeTextureFaceCount) * static_cast<uint16>(GraphicsMaxDefine::kMaxTextureMipMapCount)];

  for (auto face_index = 0; face_index < texture_desc.ArraySize; ++face_index) {
    for (auto mipmap_index = 0; mipmap_index < texture_desc.MipLevels; ++mipmap_index) {
      auto &current = data[data_count++];
      current.pSysMem = config.data[face_index][mipmap_index]->buffer();
      current.SysMemPitch = RowPitchForPixelFormat(config.color_format, config.width >> mipmap_index);
      current.SysMemSlicePitch = 0;
    }
  }

  x_d3d11_assert_msg(device_->CreateTexture2D(
    &texture_desc,
    data,
    &texture
  ), "create texture failed!\n");

  resource.texture = texture;

  ID3D11ShaderResourceView *shader_resource_view = nullptr;

  D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc;
  ZeroMemory(&shader_resource_view_desc, sizeof(shader_resource_view_desc));
  shader_resource_view_desc.Format = texture_desc.Format;
  shader_resource_view_desc.ViewDimension = config.type == TextureType::kTextureCube ? D3D11_SRV_DIMENSION_TEXTURECUBE : D3D11_SRV_DIMENSION_TEXTURE2D;
  shader_resource_view_desc.TextureCube.MostDetailedMip = 0;
  shader_resource_view_desc.TextureCube.MipLevels = texture_desc.MipLevels;
  x_d3d11_assert_msg(device_->CreateShaderResourceView(
    resource.texture,
    &shader_resource_view_desc,
    &shader_resource_view
  ), "create shader resource view failed\n");

  resource.shader_resource_view = shader_resource_view;

  resource.Complete();
}

void D3D11TextureFactory::Destroy(D3D11Texture &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.texture != nullptr) {
    resource.texture->Release();
  }
  if (resource.render_target_view != nullptr) {
    resource.render_target_view->Release();
  }
  if (resource.depth_stencil_view != nullptr) {
    resource.depth_stencil_view->Release();
  }
  if (resource.shader_resource_view != nullptr) {
    resource.shader_resource_view->Release();
  }
}

} // namespace xEngine

#endif // X_D3D11
