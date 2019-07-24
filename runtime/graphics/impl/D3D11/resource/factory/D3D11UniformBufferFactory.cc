#if X_D3D11

#include "D3D11UniformBufferFactory.h"

#include "graphics/impl/D3D11/D3D11Define.h"

#include "core/Log.h"

namespace xEngine {

  void D3D11UniformBufferFactory::Create(D3D11UniformBuffer &resource) {
    x_assert(resource.status() == ResourceStatus::kPending);
    resource.Loading();

    ID3D11Buffer *uniform_buffer = nullptr;

    D3D11_BUFFER_DESC uniform_buffer_desc;
    ZeroMemory(&uniform_buffer_desc, sizeof(uniform_buffer_desc));

    uniform_buffer_desc.ByteWidth = resource.config().size;
    uniform_buffer_desc.Usage = D3D11UsageForBufferUsage(resource.config().usage);
    uniform_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    uniform_buffer_desc.CPUAccessFlags = D3D11CPUAccessFlagForBufferUsage(resource.config().usage);

    x_d3d11_assert_msg(device_->CreateBuffer(&uniform_buffer_desc, nullptr, &uniform_buffer), "create uniform buffer failed\n");

    resource.uniform_buffer = uniform_buffer;

    resource.Complete();
  }

  void D3D11UniformBufferFactory::Destroy(D3D11UniformBuffer &resource) {
    x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
    if (resource.uniform_buffer != nullptr) {
      resource.uniform_buffer->Release();
    }
  }

} // namespace xEngine

#endif // X_D3D11