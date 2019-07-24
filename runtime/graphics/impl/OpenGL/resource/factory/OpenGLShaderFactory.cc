#if X_OPENGL

#include "OpenGLShaderFactory.h"
#include "graphics/impl/OpenGL/OpenGLDefine.h"

#include "core/Log.h"

#include <EASTL/allocator.h>

namespace xEngine {

static GLuint CompileShader(GLenum type, const char *data) {
  auto length = static_cast<GLint>(strlen(data));

  auto shader = glCreateShader(type);
  x_assert(shader != 0);

  glShaderSource(shader, 1, &data, &length);
  glCompileShader(shader);

  auto status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

#if X_DEBUG
  if (status == GL_FALSE) {
    auto log_length = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
    auto log = static_cast<char *>(eastl::GetDefaultAllocator()->allocate(static_cast<size_t>(log_length)));
    glGetShaderInfoLog(shader, log_length, &log_length, log);
    Log::GetInstance().Debug("[compile fail]:\n%s\n%s\n", data, log);
    eastl::GetDefaultAllocator()->deallocate(log, static_cast<size_t>(log_length));
  }
#endif

  if (status == GL_FALSE) {
    glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

static void DeleteShader(GLuint shader) {
  if (shader != 0) {
    glDeleteShader(shader);
  }
}

void OpenGLShaderFactory::Create(OpenGLShader &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  auto vertex_shader = CompileShader(GL_VERTEX_SHADER, resource.config().source.c_str());
  auto fragment_shader = CompileShader(GL_FRAGMENT_SHADER, resource.config().source.c_str());

  auto program = glCreateProgram();
  x_assert(program != 0);

  if (vertex_shader != 0 && fragment_shader != 0) {
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);

    for (auto semantic_index = 0; semantic_index < static_cast<uint8>(VertexElementSemantic::kMaxSemanticCount); ++semantic_index) {
      glBindAttribLocation(program,
                           static_cast<GLuint>(semantic_index),
                           AttributeNameForVertexElementSemantic(static_cast<VertexElementSemantic>(semantic_index)));
    }

    glLinkProgram(program);

    auto status = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

#if X_DEBUG
    if (status == GL_FALSE) {
      auto log_length = 0;
      glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
      auto log = static_cast<char *>(eastl::GetDefaultAllocator()->allocate(static_cast<size_t>(log_length)));
      glGetProgramInfoLog(program, log_length, &log_length, log);
      Log::GetInstance().Debug("[link fail]:\n%s\n", log);
      eastl::GetDefaultAllocator()->deallocate(log, static_cast<size_t>(log_length));
    }
#endif

    if (status == GL_FALSE) {
      glDeleteProgram(program);
      program = 0;
    }
  } else {
    glDeleteProgram(program);
    program = 0;
  }

  DeleteShader(vertex_shader);
  DeleteShader(fragment_shader);

  if (program != 0) {
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);

    glUseProgram(program);

    int32 active_uniform_block_count, max_uniform_block_name_length;

    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCKS, &active_uniform_block_count);
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &max_uniform_block_name_length);

    auto uniform_block_name = static_cast<char *>(eastl::GetDefaultAllocator()->allocate(static_cast<size_t>(max_uniform_block_name_length)));

    for (auto uniform_block_index = 0; uniform_block_index < active_uniform_block_count; ++uniform_block_index) {
      OpenGLShader::UniformBlockInfo info;
      GLsizei actual_uniform_block_name_length;
      glGetActiveUniformBlockName(program, static_cast<GLuint>(uniform_block_index), max_uniform_block_name_length, &actual_uniform_block_name_length, uniform_block_name);
      info.location = static_cast<GLuint>(uniform_block_index);
      glGetActiveUniformBlockiv(program, info.location, GL_UNIFORM_BLOCK_DATA_SIZE, &info.size);
      glUniformBlockBinding(program, info.location, info.location);
      resource.uniform_block_info.insert(eastl::make_pair(eastl::string(uniform_block_name, static_cast<size_t>(actual_uniform_block_name_length)), info));
    }

    eastl::GetDefaultAllocator()->deallocate(uniform_block_name, static_cast<size_t>(max_uniform_block_name_length));

    int32 active_uniform_count, max_uniform_name_length;

    glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &active_uniform_count);
    glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);

    auto uniform_name = static_cast<char *>(eastl::GetDefaultAllocator()->allocate(static_cast<size_t>(max_uniform_name_length)));

    uint8 texture_cube_location = 0;
    uint8 texture_2d_location = 0;
    for (auto uniform_index = 0; uniform_index < active_uniform_count; ++uniform_index) {
      OpenGLShader::UniformInfo info;
      GLsizei actual_uniform_name_length;
      glGetActiveUniform(program, static_cast<GLuint>(uniform_index), max_uniform_name_length, &actual_uniform_name_length, &info.size, &info.type, uniform_name);
      info.location = static_cast<GLuint>(glGetUniformLocation(program, uniform_name));
      if (info.type == GL_SAMPLER_2D) {
        info.texture_2d_index = texture_2d_location;
        glUniform1i(info.location, texture_2d_location);
        ++texture_2d_location;
      } else if (info.type == GL_SAMPLER_CUBE) {
        info.texture_cube_index = texture_cube_location;
        glUniform1i(info.location, texture_cube_location);
        ++texture_cube_location;
      }
      resource.uniform_info.insert(eastl::make_pair(eastl::string(uniform_name, static_cast<size_t>(actual_uniform_name_length)), info));
    }

    eastl::GetDefaultAllocator()->deallocate(uniform_name, static_cast<size_t>(max_uniform_name_length));

    glUseProgram(static_cast<GLuint>(current_program));
  }

  resource.program_id = program;

  resource.program_id == 0 ? resource.Failed() : resource.Complete();
}

void OpenGLShaderFactory::Destroy(OpenGLShader &resource) {
  x_assert(resource.status() == ResourceStatus::kCompleted || resource.status() == ResourceStatus::kFailed);
  if (resource.program_id != 0) {
    glDeleteProgram(resource.program_id);
  }
}

} // namespace xEngine

#endif // X_OPENGL
