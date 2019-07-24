#include "Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>
#include <gtx/transform.hpp>

namespace xEngine {

void Transform::FromMatrix(const glm::mat4 &matrix) {
  glm::vec3 skew;
  glm::vec4 perspective;
  glm::decompose(matrix, scale_, rotation_, translation_, skew, perspective);
}

glm::mat4 Transform::ToMatrix() const {
  if (parent_ == nullptr) {
    return glm::translate(translation_) * glm::mat4_cast(rotation_) * glm::scale(scale_);
  }
  return parent_->ToMatrix() * (glm::translate(translation_) * glm::mat4_cast(rotation_) * glm::scale(scale_));
}

void Transform::Serialize() {

}

void Transform::Deserialize() {

}

} // namespace xEngine