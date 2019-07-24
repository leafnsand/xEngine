#ifndef XENGINE_OBJECT_CORE_GAMEOBJECT_H
#define XENGINE_OBJECT_CORE_GAMEOBJECT_H

#include "Component.h"

#include "core/Core.h"

#include <EASTL/list.h>

namespace xEngine {

class GameObject {
 public:
  CREATE_FUNC_DECLARE(GameObject)

  GameObject();

  ~GameObject();

  ComponentPtr AttachComponent(ComponentType type);

  void DetachComponent(ComponentPtr component);

  template <class T>
  eastl::shared_ptr<T> GetComponent(ComponentType type);

  void Update();

  void Serialize();

  void Deserialize();

  uint32 id() const { return id_; }

 private:
  uint32 id_{0};
  eastl::list<ComponentPtr> components_;
};

PTR_DECLARE(GameObject)

template <class T>
eastl::shared_ptr<T> GameObject::GetComponent(ComponentType type) {
  for (auto &component : components_) {
    if (component->type() == type) {
      return eastl::static_pointer_cast<T>(component);
    }
  }
  return nullptr;
}

} // namespace xEngine

#endif // XENGINE_OBJECT_CORE_GAMEOBJECT_H
