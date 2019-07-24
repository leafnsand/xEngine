#if X_OPENGL

#include "OpenGLMeshFactory.h"

#include "graphics/impl/OpenGL/OpenGLDefine.h"

#include "core/Log.h"

namespace xEngine {

void OpenGLMeshFactory::Create(OpenGLMesh &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  auto &layout = resource.config().layout;
  if (layout.size == 0 || resource.config().vertex_count == 0) {
    Log::GetInstance().Error("[OpenGLMeshFactory::Create] vertex data wrong size\n");
    resource.Failed();
    return;
  }

  GLuint vertex_array;
  glGenVertexArrays(1, &vertex_array);
  x_assert(vertex_array != 0);

  GLint current_vertex_array;
  glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &current_vertex_array);

  glBindVertexArray(vertex_array);

  GLuint vertex_buffer;
  glGenBuffers(1, &vertex_buffer);
  x_assert(vertex_buffer != 0);

  GLint current_vertex_buffer;
  glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &current_vertex_buffer);

  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

  glBufferData(GL_ARRAY_BUFFER,
               resource.config().vertex_count * resource.config().layout.size,
               resource.config().vertex_data ? resource.config().vertex_data->buffer() : nullptr,
               GLEnumForBufferUsage(resource.config().vertex_usage));

  for (auto index = 0; index < layout.element_count; ++index) {
    auto &element = layout.elements[index];
    if (element.semantic != VertexElementSemantic::kInvalid) {
      glEnableVertexAttribArray(static_cast<GLuint>(element.semantic));
      glVertexAttribPointer(static_cast<GLuint>(element.semantic),
                            VertexCountForVertexElementFormat(element.format),
                            GLEnumForVertexElementFormat(element.format),
                            static_cast<GLboolean>(IsNormalizedForVertexElementFormat(element.format)),
                            static_cast<GLsizei>(layout.size),
                            reinterpret_cast<GLvoid *>(element.offset));
    }
  }

  GLint current_index_buffer = -1;
  glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &current_index_buffer);

  if (resource.config().index_type != IndexFormat::kNone && resource.config().index_count > 0) {
    GLuint index_buffer;
    glGenBuffers(1, &index_buffer);
    x_assert(index_buffer != 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 SizeOfIndexFormat(resource.config().index_type) * resource.config().index_count,
                 resource.config().index_data ? resource.config().index_data->buffer() : nullptr,
                 GLEnumForBufferUsage(resource.config().index_usage));

    resource.config().index_data = nullptr;

    resource.index_buffer_id = index_buffer;
  }

  glBindVertexArray(static_cast<GLuint>(current_vertex_array));

  glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(current_vertex_buffer));

  if (current_index_buffer != -1) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLuint>(current_index_buffer));
  }

  resource.config().vertex_data = nullptr;

  resource.vertex_buffer_id = vertex_buffer;

  resource.vertex_array_id = vertex_array;

  resource.Complete();
}
void OpenGLMeshFactory::Destroy(OpenGLMesh &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.vertex_buffer_id != 0) {
    glDeleteBuffers(1, &resource.vertex_buffer_id);
  }
  if (resource.index_buffer_id != 0) {
    glDeleteBuffers(1, &resource.index_buffer_id);
  }
  if (resource.vertex_array_id != 0) {
    glDeleteVertexArrays(1, &resource.vertex_array_id);
  }
}

} // namespace xEngine

#endif // X_OPENGL