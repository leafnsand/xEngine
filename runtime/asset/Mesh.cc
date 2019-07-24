#include "Mesh.h"

#include "window/Window.h"

namespace xEngine {

MeshPtr Mesh::Parse(ResourceID window, DataPtr data) {
  if (window == kInvalidResourceID || data == nullptr) return nullptr;

  auto pointer = data->buffer();
  auto data_available = data->size();

  if (data_available < sizeof(uint8)) {
    Log::GetInstance().Error("load mesh error, no semantic count!\n");
    return nullptr;
  }

  auto mesh = Mesh::Create();

  auto semantic_count = read<uint8>(&pointer);

  data_available -= sizeof(uint8);

  for (auto semantic_index = 0; semantic_index < semantic_count; ++semantic_index) {
    if (data_available < sizeof(uint8) * 2) {
      Log::GetInstance().Error("load mesh error, no vertex element info of %d!\n", semantic_index + 1);
      return nullptr;
    }

    auto semantic_type = static_cast<VertexElementSemantic>(read<uint8>(&pointer));
    auto semantic_format = static_cast<VertexElementFormat>(read<uint8>(&pointer));
    mesh->config_.layout.AddElement(semantic_type, semantic_format);

    data_available -= sizeof(uint8) * 2;
  }

  if (data_available < sizeof(size_t)) {
    Log::GetInstance().Error("load mesh error, no vertex count!\n");
    return nullptr;
  }
  
  auto vertex_count = read<size_t>(&pointer);
  mesh->config_.vertex_count = vertex_count;

  data_available -= sizeof(size_t);

  if (data_available < sizeof(uint8)) {
    Log::GetInstance().Error("load mesh error, no index type!\n");
    return nullptr;
  }
  
  auto index_type = static_cast<IndexFormat>(read<uint8>(&pointer));
  mesh->config_.index_type = index_type;

  data_available -= sizeof(uint8);

  if (data_available < sizeof(size_t)) {
    Log::GetInstance().Error("load mesh error, no index count!\n");
    return nullptr;
  }
  
  auto index_count = read<size_t>(&pointer);
  mesh->config_.index_count = index_count;

  data_available -= sizeof(size_t);

  if (data_available < mesh->config_.layout.size * mesh->config_.vertex_count) {
    Log::GetInstance().Error("load mesh error, no vertex data!\n");
    return nullptr;
  }

  auto vertex_data = Data::Create(mesh->config_.layout.size * mesh->config_.vertex_count);
  memcpy(vertex_data->buffer(), pointer, mesh->config_.layout.size * mesh->config_.vertex_count);
  mesh->config_.vertex_data = vertex_data;

  data_available -= mesh->config_.layout.size * mesh->config_.vertex_count;

  if (data_available < SizeOfIndexFormat(mesh->config_.index_type) * mesh->config_.index_count) {
    Log::GetInstance().Error("load mesh error, no vertex data!\n");
    return nullptr;
  }

  auto index_data = Data::Create(SizeOfIndexFormat(mesh->config_.index_type) * mesh->config_.index_count);
  memcpy(index_data->buffer(), pointer, SizeOfIndexFormat(mesh->config_.index_type) * mesh->config_.index_count);
  mesh->config_.index_data = index_data;

  data_available -= SizeOfIndexFormat(mesh->config_.index_type) * mesh->config_.index_count;

  if (data_available != 0) {
    Log::GetInstance().Warning("load mesh, invalid data, size: %d!\n", data_available);
  }

  mesh->window_id_ = window;
  mesh->draw_call_state_ = DrawCallState::Triangles(mesh->config_.index_count);

  return mesh;
}

MeshPtr Mesh::Parse(ResourceID window, MeshUtil util) {
  auto mesh = Mesh::Create();
  mesh->window_id_ = window;
  mesh->config_ = util.config();
  mesh->draw_call_state_ = DrawCallState::Triangles(util.config().index_count);

  return mesh;

}

void Mesh::Initialize() {
  mesh_id_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(config_);
}

void Mesh::Finalize() {
  Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Destroy(mesh_id_);
}

void Mesh::Apply() {
  auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
  renderer->ApplyMesh(mesh_id_);
  renderer->Draw(draw_call_state_);
}

} // namespace xEngine
