#ifndef XENGINE_OBJECT_COMPONENT_TRANSFORM_H
#define XENGINE_OBJECT_COMPONENT_TRANSFORM_H

#include "object/core/Component.h"

#include "core/Core.h"

#include <glm.hpp>
#include <gtc/quaternion.hpp>

#include <EASTL/list.h>

namespace xEngine {

class Transform;

PTR_DECLARE(Transform)

class Transform : public Component {
 public:
  CREATE_FUNC_DECLARE(Transform)

  explicit Transform(GameObject &game_object) : Component(game_object) {}

  virtual ~Transform() {}

  void AddChild(const TransformPtr &child) { children_.push_back(child); }

  void FromMatrix(const glm::mat4 &matrix);

  glm::mat4 ToMatrix() const;

  virtual void Serialize() override;

  virtual void Deserialize() override;

  virtual ComponentType type() override { return ComponentType::kTransform; }

  void set_parent(const TransformPtr &value) { parent_ = value; }

  const TransformPtr &parent() const { return parent_; }

  const eastl::list<TransformPtr> &children() const { return children_; }

  const glm::quat &rotation() const { return rotation_; }

  const glm::vec3 &translation() const { return translation_; }

  const glm::vec3 &scale() const { return scale_; }

 private:
  TransformPtr parent_;
  eastl::list<TransformPtr> children_;

  glm::quat rotation_;
  glm::vec3 translation_;
  glm::vec3 scale_{1.0f, 1.0f, 1.0f};
};

} // namespace xEngine

#endif // XENGINE_OBJECT_COMPONENT_TRANSFORM_H
