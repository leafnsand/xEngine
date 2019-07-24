#if X_OPENGL

#include "OpenGLSamplerFactory.h"

#include "graphics/impl/OpenGL/OpenGLDefine.h"

#include "core/Log.h"

namespace xEngine {

void OpenGLSamplerFactory::Create(OpenGLSampler &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();
  
  auto &config = resource.config();

  if (config.filter_mode_mag != TextureFilterMode::kLinear && config.filter_mode_mag != TextureFilterMode::kNearest) {
    Log::GetInstance().Error("[OpenGLSamplerFactory::Create] wrong filter mode mag\n");
    resource.Failed();
    return;
  }

  GLuint sampler_id;
  glGenSamplers(1, &sampler_id);
  x_assert(sampler_id != 0);
  
  glSamplerParameteri(sampler_id, GL_TEXTURE_MIN_FILTER, GLEnumForTextureFilterMode(config.filter_mode_min));
  glSamplerParameteri(sampler_id, GL_TEXTURE_MAG_FILTER, GLEnumForTextureFilterMode(config.filter_mode_mag));

  glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_S, GLEnumForTextureWrapMode(config.wrap_mod_s));
  glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_T, GLEnumForTextureWrapMode(config.wrap_mod_t));
  glSamplerParameteri(sampler_id, GL_TEXTURE_WRAP_R, GLEnumForTextureWrapMode(config.wrap_mod_r));

  glSamplerParameterf(sampler_id, GL_TEXTURE_MIN_LOD, config.min_lod);
  glSamplerParameterf(sampler_id, GL_TEXTURE_MAX_LOD, config.max_lod);

  glSamplerParameterf(sampler_id, GL_TEXTURE_LOD_BIAS, config.lod_bias);

  glSamplerParameterfv(sampler_id, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(config.border_color));

  glSamplerParameteri(sampler_id, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glSamplerParameteri(sampler_id, GL_TEXTURE_COMPARE_FUNC, GLEnumForCompareFunction(config.compare));

  resource.sampler_id = sampler_id;

  resource.Complete();
}
void OpenGLSamplerFactory::Destroy(OpenGLSampler &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.sampler_id != 0) {
    glDeleteSamplers(1, &resource.sampler_id);
  }
}

} // namespace xEngine

#endif // X_OPENGL