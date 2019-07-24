#if X_OPENGL

#include "OpenGLUniformBufferFactory.h"

#include "graphics/impl/OpenGL/OpenGLDefine.h"

#include "core/Log.h"

namespace xEngine {

void OpenGLUniformBufferFactory::Create(OpenGLUniformBuffer &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  GLuint uniform_buffer_id;
  glGenBuffers(1, &uniform_buffer_id);
  x_assert(uniform_buffer_id != 0);

  GLint current_uniform_buffer;
  glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &current_uniform_buffer);

  glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer_id);
  glBufferData(GL_UNIFORM_BUFFER, resource.config().size, nullptr, GLEnumForBufferUsage(resource.config().usage));

  glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(current_uniform_buffer));

  resource.uniform_buffer_id = uniform_buffer_id;

  resource.Complete();
}
void OpenGLUniformBufferFactory::Destroy(OpenGLUniformBuffer &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.uniform_buffer_id != 0) {
    glDeleteBuffers(1, &resource.uniform_buffer_id);
  }
}

} // namespace xEngine

#endif // X_OPENGL