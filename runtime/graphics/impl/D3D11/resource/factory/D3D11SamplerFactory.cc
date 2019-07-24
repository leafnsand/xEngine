#if X_D3D11

#include "D3D11SamplerFactory.h"

#include "graphics/impl/D3D11/D3D11Define.h"

#include "core/Log.h"

namespace xEngine {

  void D3D11SamplerFactory::Create(D3D11Sampler &resource) {
    x_assert(resource.status() == ResourceStatus::kPending);
    resource.Loading();

    auto &config = resource.config();

    ID3D11SamplerState *sampler_state = nullptr;

    D3D11_SAMPLER_DESC sampler_desc;
    ZeroMemory(&sampler_desc, sizeof(sampler_desc));
    sampler_desc.Filter = EnumForTextureFilterMode(config.filter_mode_min, config.filter_mode_mag);
    sampler_desc.AddressU = EnumForTextureWrapMode(config.wrap_mod_s);
    sampler_desc.AddressV = EnumForTextureWrapMode(config.wrap_mod_t);
    sampler_desc.AddressW = EnumForTextureWrapMode(config.wrap_mod_r);
    sampler_desc.MipLODBias = config.lod_bias;
    sampler_desc.MaxAnisotropy = 1;
    sampler_desc.ComparisonFunc = EnumForCompareFunction(config.compare);
    sampler_desc.BorderColor[0] = config.border_color.r;
    sampler_desc.BorderColor[1] = config.border_color.g;
    sampler_desc.BorderColor[2] = config.border_color.b;
    sampler_desc.BorderColor[3] = config.border_color.a;
    sampler_desc.MinLOD = config.min_lod;
    sampler_desc.MaxLOD = config.max_lod;
    x_d3d11_assert_msg(device_->CreateSamplerState(&sampler_desc, &sampler_state), "create sampler state failed\n");

    resource.sampler_state = sampler_state;

    resource.Complete();
  }

  void D3D11SamplerFactory::Destroy(D3D11Sampler &resource) {
    x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
    if (resource.sampler_state != nullptr) {
      resource.sampler_state->Release();
    }
  }

} // namespace xEngine

#endif // X_D3D11