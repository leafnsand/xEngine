#include "GameObject.h"

#include "object/component/Transform.h"
#include "object/component/Camera.h"
#include "object/component/MeshRenderer.h"

namespace xEngine {

GameObject::GameObject() {
  static uint32 global_id = 0;
  id_ = ++global_id;
  components_.push_back(Transform::Create(*this));
}

GameObject::~GameObject() {

}

ComponentPtr GameObject::AttachComponent(ComponentType type) {
  ComponentPtr component;
  switch (type) {
    case ComponentType::kTransform:
      component = Transform::Create(*this);
      break;
    case ComponentType::kCamera:
      component = Camera::Create(*this);
      break;
    case ComponentType::kMeshRenderer:
      component = MeshRenderer::Create(*this);
      break;
    default:
      x_error("invalid component type");
      break;
  }
  components_.push_back(component);
  return component;
}

void GameObject::DetachComponent(ComponentPtr component) {
  if (eastl::find(components_.begin(), components_.end(), component) == components_.end()) return;
  components_.remove(component);
}

void GameObject::Update() {
  for (auto &component : components_) {
    component->Update();
  }
}

void GameObject::Serialize() {

}

void GameObject::Deserialize() {

}

} // namespace xEngine
