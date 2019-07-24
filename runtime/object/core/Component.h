#ifndef XENGINE_OBJECT_CORE_COMPONENT_H
#define XENGINE_OBJECT_CORE_COMPONENT_H

#include <core/Core.h>
#include <core/Types.h>

namespace xEngine {

class GameObject;

class Component;

PTR_DECLARE(Component)

enum class ComponentType : uint8 {
  kInvalid,
  kTransform,
  kCamera,
  kMeshRenderer,
};

class Component {
 public:
  Component(GameObject &game_object) : game_object_(game_object) {}

  virtual ~Component() {}

  virtual void Update() {}

  virtual void Serialize() = 0;

  virtual void Deserialize() = 0;

  virtual ComponentType type() { return ComponentType::kInvalid; }

  GameObject &game_object() const { return game_object_; }

 private:
  GameObject &game_object_;
};

} // namespace xEngine

namespace eastl {

template<>
struct hash<xEngine::ComponentType> {
  size_t operator()(const xEngine::ComponentType &other) const { return static_cast<size_t>(other); }
};

} // namespace eastl

#endif // XENGINE_OBJECT_CORE_COMPONENT_H
