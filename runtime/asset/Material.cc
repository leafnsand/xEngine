#include "Material.h"

namespace xEngine {

void Material::Apply(const glm::mat4 &matrix) const {
  if (shader_ != nullptr) {
    shader_->Apply();
    shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(matrix), sizeof(matrix)));
  }
}

void Material::BindTexture(const eastl::string &name, TexturePtr texture) const {
  if (shader_ != nullptr && texture != nullptr) {
    shader_->UpdateResourceTexture(name, texture->texture_id());
    shader_->UpdateResourceSampler(name + "_sampler", texture->sampler_id());
  }
}

} // namespace xEngine