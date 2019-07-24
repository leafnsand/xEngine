#include "MeshRenderer.h"
#include "Transform.h"

#include "object/core/GameObject.h"

namespace xEngine {

void MeshRenderer::Update() {
  auto matrix = game_object().GetComponent<Transform>(ComponentType::kTransform)->ToMatrix();
  for (auto &material : materials_) {
    if (material == nullptr) continue;
    material->Apply(matrix);
    if (mesh_ == nullptr) continue;
    mesh_->Apply();
  }
}

void MeshRenderer::Serialize() {

}

void MeshRenderer::Deserialize() {

}

} // namespace xEngine