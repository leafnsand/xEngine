#ifndef XENGINE_OBJECT_COMPONENT_CAMERA_H
#define XENGINE_OBJECT_COMPONENT_CAMERA_H

#include "object/core/Component.h"

#include "core/Core.h"
#include "core/Types.h"
#include "graphics/state/ClearState.h"
#include "resource/Resource.h"

#include <glm.hpp>

#pragma push_macro("near")
#pragma push_macro("far")

#undef near
#undef far

namespace xEngine {

enum class CameraClearType {
  kSkyBox,
  kSolidColor,
  kDepthStencil,
  kNone,
};

inline ClearType FromCameraClearType(CameraClearType type) {
  switch (type) {
    case CameraClearType::kSkyBox:
    case CameraClearType::kSolidColor:
      return ClearType::kColor;
    case CameraClearType::kDepthStencil:
      return ClearType::kDepthAndStencil;
    default:
      return ClearType::kNone;
  }
}

enum class CameraType {
  kPerspective,
  kOrthographic,
};

class Camera;

PTR_DECLARE(Camera)

class Camera : public Component {
 public:
  static CameraPtr main_camera() { return main_camera_; }

  static void set_main_camera(CameraPtr camera) { main_camera_ = camera; }

 private:
  static CameraPtr main_camera_;

 public:
  CREATE_FUNC_DECLARE(Camera)

  explicit Camera(GameObject &game_object) : Component(game_object) {}

  virtual ~Camera() {}

  void Rotate(float32 yaw, float32 pitch);

  void Move(float32 x, float32 y);

  void set_zoom(float32 value) { zoom_ = glm::max(0.0f, value); UpdateProjectionMatrix(); }

  float32 zoom() const { return zoom_; }

  void set_render_target(ResourceID value) { render_target_ = value; UpdateProjectionMatrix(); }

  ResourceID render_target() const { return render_target_; }

  void set_clear_type(CameraClearType type) { clear_state_.type = FromCameraClearType(type); }

  void set_background_color(Color color) { clear_state_.clear_color = color; }

  Color background_color() const { return clear_state_.clear_color; }

  void set_type(CameraType value) { type_ = value; UpdateProjectionMatrix(); }

  CameraType type() const { return type_; }

  void set_fov(float32 value) { fov_ = value; UpdateProjectionMatrix(); }

  float32 fov() const { return fov_; }

  void set_size(float32 value) { size_ = value; UpdateProjectionMatrix(); }

  float32 size() const { return size_; }

  void set_near(float32 value) { near_ = value; UpdateProjectionMatrix(); }

  float32 near() const { return near_; }

  void set_far(float32 value) { far_ = value; UpdateProjectionMatrix(); }

  float32 far() const { return far_; }

  const glm::mat4 &projection_matrix() const { return projection_matrix_; }

  void set_up_direction(const glm::vec3 &value) { up_direction_ = value; UpdateViewMatrix(); }

  const glm::vec3 &up_direction() const { return up_direction_; }

  void set_position(const glm::vec3 &value) { position_ = value; UpdateViewMatrix(); }

  const glm::vec3 &positon() const { return position_; }

  void set_target(const glm::vec3 &value) { target_ = value; UpdateViewMatrix(); }

  const glm::vec3 &target() const { return target_; }

  const glm::mat4 &view_matrix() const { return view_matrix_; }

  virtual void Update() override;

  virtual void Serialize() override;

  virtual void Deserialize() override;

  virtual ComponentType type() override { return ComponentType::kCamera; }

 private:
  void UpdateProjectionMatrix();

  void UpdateViewMatrix();

 private:
  float32 zoom_{1.0f};

  ResourceID render_target_{kInvalidResourceID};
  ClearState clear_state_;

  CameraType type_{CameraType::kPerspective};

  float32 fov_{60.0f}; // perspective
  float32 size_{5.0f}; // orthographic

  float32 near_{1.0f};
  float32 far_{1000.0f};

  glm::mat4 projection_matrix_;

  glm::vec3 up_direction_;
  glm::vec3 position_;
  glm::vec3 target_;

  glm::mat4 view_matrix_;
};

} // namespace xEngine

#pragma pop_macro("near")
#pragma pop_macro("far")

#endif // XENGINE_OBJECT_COMPONENT_CAMERA_H
