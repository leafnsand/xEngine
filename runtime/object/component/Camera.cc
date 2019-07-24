#include "Camera.h"

#include "window/Window.h"

#include <gtc/matrix_transform.hpp>

namespace xEngine {

CameraPtr Camera::main_camera_;

void Camera::Rotate(float32 yaw, float32 pitch) {
  auto eye_direction = target_ - position_;
  auto rotation = glm::rotate(glm::mat4(), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
  rotation = glm::rotate(rotation, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
  target_ = glm::vec3(rotation * glm::vec4(eye_direction, 0.0f)) + position_;
  UpdateViewMatrix();
}

void Camera::Move(float32 x, float32 y) {
  auto eye_direction = target_ - position_;
  auto right_direction = glm::cross(up_direction_, eye_direction);
  position_ = position_ + x * glm::normalize(right_direction) + y * glm::normalize(up_direction_);
  target_ = position_ + eye_direction;
  UpdateViewMatrix();
}

void Camera::UpdateProjectionMatrix() {
  if (render_target_ == kInvalidResourceID) return;

  auto &config = Window::GetInstance().Get(render_target_)->config(); // TODO just window for now

  if (type_ == CameraType::kPerspective) {
    auto aspect = static_cast<float32>(config.width) / static_cast<float32>(config.height);
    projection_matrix_ = glm::perspective(glm::radians(fov_ / zoom_), aspect, near_, far_);
  } else if (type_ == CameraType::kOrthographic) {
    auto half_width =  0.5f * static_cast<float32>(config.width) / (zoom_ * size_);
    auto half_height = 0.5f * static_cast<float32>(config.height) / (zoom_ * size_);
    projection_matrix_ = glm::ortho(-half_width, +half_width, -half_height, +half_height, near_, far_);
  }
}

void Camera::UpdateViewMatrix() {
  view_matrix_ = glm::lookAt(position_, target_, up_direction_);
}

void Camera::Update() {
  if (render_target_ == kInvalidResourceID) return;

  // TODO just window for now
  auto &renderer = Window::GetInstance().GetGraphics(render_target_)->renderer();
  renderer->Render(); // invert render & apply call order to make camera update once per frame
  renderer->ApplyTarget(kInvalidResourceID, clear_state_);

  // TODO SkyBox render
}

void Camera::Serialize() {

}

void Camera::Deserialize() {

}

} // namespace xEngine
