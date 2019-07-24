#if X_D3D11

#include "D3D11MeshFactory.h"

#include "graphics/impl/D3D11/D3D11Define.h"

#include "core/Log.h"

namespace xEngine {

void D3D11MeshFactory::Create(D3D11Mesh &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  if (resource.config().layout.element_count == 0 || resource.config().vertex_count == 0) {
    Log::GetInstance().Error("[D3D11MeshFactory::Create] vertex data wrong size\n");
    resource.Failed();
    return;
  }

  if (resource.config().index_type != IndexFormat::kNone && resource.config().index_count > 0) {
    ID3D11Buffer *index_buffer = nullptr;

    D3D11_BUFFER_DESC index_desc;
    ZeroMemory(&index_desc, sizeof(index_desc));
    index_desc.ByteWidth = SizeOfIndexFormat(resource.config().index_type) * resource.config().index_count;
    index_desc.Usage = D3D11UsageForBufferUsage(resource.config().index_usage);
    index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_desc.CPUAccessFlags = D3D11CPUAccessFlagForBufferUsage(resource.config().index_usage);
    index_desc.MiscFlags = 0;
    index_desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA index_data;
    index_data.pSysMem = resource.config().index_data == nullptr ? nullptr : resource.config().index_data->buffer();
    index_data.SysMemPitch = 0;
    index_data.SysMemSlicePitch = 0;

    x_d3d11_assert_msg(device_->CreateBuffer(
      &index_desc,
      index_data.pSysMem == nullptr ? nullptr : &index_data,
      &index_buffer
    ), "create index buffer failed\n");

    resource.index_buffer = index_buffer;
  }

  ID3D11Buffer *vertex_buffer = nullptr;

  D3D11_BUFFER_DESC vertex_desc;
  ZeroMemory(&vertex_desc, sizeof(vertex_desc));
  vertex_desc.ByteWidth = resource.config().layout.size * resource.config().vertex_count;
  vertex_desc.Usage = D3D11UsageForBufferUsage(resource.config().vertex_usage);
  vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertex_desc.CPUAccessFlags = D3D11CPUAccessFlagForBufferUsage(resource.config().vertex_usage);
  vertex_desc.MiscFlags = 0;
  vertex_desc.StructureByteStride = 0;

  D3D11_SUBRESOURCE_DATA vertex_data;
  vertex_data.pSysMem = resource.config().vertex_data == nullptr ? nullptr : resource.config().vertex_data->buffer();
  vertex_data.SysMemPitch = 0;
  vertex_data.SysMemSlicePitch = 0;

  x_d3d11_assert_msg(device_->CreateBuffer(
    &vertex_desc,
    vertex_data.pSysMem == nullptr ? nullptr : &vertex_data,
    &vertex_buffer
  ), "create vertex buffer failed\n");

  resource.vertex_buffer = vertex_buffer;

  resource.Complete();
}

void D3D11MeshFactory::Destroy(D3D11Mesh &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.index_buffer != nullptr) {
    resource.index_buffer->Release();
  }
  if (resource.vertex_buffer != nullptr) {
    resource.vertex_buffer->Release();
  }
}

} // namespace xEngine

#endif // X_D3D11