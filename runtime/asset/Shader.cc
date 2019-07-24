#include "Shader.h"

#include "object/component/Camera.h"

#include "window/Window.h"

namespace xEngine {

ShaderPtr Shader::Parse(ResourceID window, DataPtr data) {
  x_assert(window != kInvalidResourceID);
  
  auto shader = Shader::Create();
  shader->window_id_ = window;
  shader->shader_config_.source = eastl::string(static_cast<char *>(data->buffer()), data->size());

  return shader;
}

void Shader::Initialize() {
  x_assert(shader_id_ == kInvalidResourceID && pipeline_id_ == kInvalidResourceID);
  shader_id_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(shader_config_);
  info_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->QueryInfo(shader_id_);
  pipeline_config_.shader = shader_id_;
  pipeline_id_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config_);
}

void Shader::Finalize() {
  x_assert(shader_id_ != kInvalidResourceID && pipeline_id_ != kInvalidResourceID);
  Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Destroy(shader_id_);
  shader_id_ = kInvalidResourceID;
  Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Destroy(pipeline_id_);
  pipeline_id_ = kInvalidResourceID;
}

void Shader::Apply() const {
  x_assert(shader_id_ != kInvalidResourceID);
  Window::GetInstance().GetGraphics(window_id_)->renderer()->ApplyPipeline(pipeline_id_);
  Window::GetInstance().GetGraphics(window_id_)->renderer()->ApplyShader(shader_id_);
  auto camera = Camera::main_camera();
  if (camera != nullptr) {
    UpdateResourceData("uView", Data::Create(glm::value_ptr(camera->view_matrix()), sizeof(camera->view_matrix())));
    UpdateResourceData("uProjection", Data::Create(glm::value_ptr(camera->projection_matrix()), sizeof(camera->projection_matrix())));
  }
}

void Shader::UpdateResourceData(const eastl::string &name, DataPtr data) const {
  x_assert(shader_id_ != kInvalidResourceID);
  Window::GetInstance().GetGraphics(window_id_)->renderer()->UpdateShaderResourceData(shader_id_, name, data);
}

void Shader::UpdateResourceTexture(const eastl::string &name, ResourceID texture_id) const {
  x_assert(shader_id_ != kInvalidResourceID);
  Window::GetInstance().GetGraphics(window_id_)->renderer()->UpdateShaderResourceTexture(shader_id_, name, texture_id);
}

void Shader::UpdateResourceSampler(const eastl::string &name, ResourceID sampler_id) const {
  x_assert(shader_id_ != kInvalidResourceID);
  Window::GetInstance().GetGraphics(window_id_)->renderer()->UpdateShaderResourceSampler(shader_id_, name, sampler_id);
}

void Shader::UpdateResourceBlock(const eastl::string &name, ResourceID uniform_buffer_id) const {
  x_assert(shader_id_ != kInvalidResourceID);
  Window::GetInstance().GetGraphics(window_id_)->renderer()->UpdateShaderResourceBlock(shader_id_, name, uniform_buffer_id);
}

} // namespace xEngine