#if X_OPENGL

#include "OpenGLRenderer.h"

namespace xEngine {

extern void *CreateOpenGLContext(const GraphicsConfig &config, void *native_handle);

extern void DestroyOpenGLContext(void *context);

extern void OpenGLContextSwapBuffers(void *context);

extern void OpenGLContextMakeCurrent(void *context);

extern void OpenGLContextSetSwapInterval(void *context, int32 interval);

void OpenGLRenderer::PreOpenGLCallback(const char *name, void *function, int len_args, ...) {
  GLenum errorCode;
  errorCode = glad_glGetError();
  if (errorCode != GL_NO_ERROR) {
    Log::GetInstance().Error("[OpenGLRenderer::PreOpenGLCallback] ERROR 0x%04x in %s\n", errorCode, name);
    X_TRAP();
  }
}

void OpenGLRenderer::PostOpenGLCallback(const char *name, void *function, int len_args, ...) {
  GLenum errorCode;
  errorCode = glad_glGetError();
  if (errorCode != GL_NO_ERROR) {
    Log::GetInstance().Error("[OpenGLRenderer::PostOpenGLCallback] ERROR 0x%04x in %s\n", errorCode, name);
    X_TRAP();
  }
}

void OpenGLRenderer::Initialize(const GraphicsConfig &config) {
  config_ = config;
  context_ = CreateOpenGLContext(config_, window()->GetNativeHandle());
  OpenGLContextMakeCurrent(context_);
  OpenGLContextSetSwapInterval(context_, config_.swap_interval);
  if (gladLoadGL() == 0) {
    x_error("OpenGL init error!\n");
  }
#ifdef GLAD_DEBUG
  glad_set_pre_callback(PreOpenGLCallback);
  glad_set_post_callback(PostOpenGLCallback);
#endif
  Reset();
}

void OpenGLRenderer::Finalize() {
  Reset();
  config_ = GraphicsConfig();
  DestroyOpenGLContext(context_);
}

void OpenGLRenderer::Render() {
  OpenGLContextSwapBuffers(context_);
}

void OpenGLRenderer::MakeCurrent() {
  OpenGLContextMakeCurrent(context_);
}

void OpenGLRenderer::ApplyTarget(ResourceID id, const ClearState &state) {
  if (id != kInvalidResourceID) {
    auto &texture = resource_manager()->texture_pool_.Find(id);
    if (texture.status() == ResourceStatus::kCompleted) {
      if (texture.texture_id != cache_.frame_buffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, texture.texture_id);
        ApplyViewPort(0, 0, texture.config().width, texture.config().height);
        cache_.frame_buffer = texture.texture_id;
      }
    }
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    ApplyViewPort(0, 0, window()->config().frame_buffer_width, window()->config().frame_buffer_height);
    cache_.frame_buffer = 0;
  }

  if (cache_.rasterizer_state.scissor_test_enable) {
    cache_.rasterizer_state.scissor_test_enable = false;
    glDisable(GL_SCISSOR_TEST);
  }

  GLbitfield clear_mask = 0;

  if (static_cast<uint8>(state.type) & static_cast<uint8>(ClearType::kColor)) {
    clear_mask |= GL_COLOR_BUFFER_BIT;
    glClearColor(state.clear_color.r, state.clear_color.g, state.clear_color.b, state.clear_color.a);
    if (cache_.blend_state.color_mask != PixelChannel::kRGBA) {
      cache_.blend_state.color_mask = PixelChannel::kRGBA;
      glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }
  }

  if (static_cast<uint8>(state.type) & static_cast<uint8>(ClearType::kDepth)) {
    clear_mask |= GL_DEPTH_BUFFER_BIT;
    glClearDepth(state.depth);
    if (!cache_.depth_stencil_state.depth_write_enable) {
      cache_.depth_stencil_state.depth_write_enable = true;
      glDepthMask(GL_TRUE);
    }
  }

  if (static_cast<uint8>(state.type) & static_cast<uint8>(ClearType::kStencil)) {
    clear_mask |= GL_STENCIL_BUFFER_BIT;
    glClearStencil(state.stencil);
    if (cache_.depth_stencil_state.stencil_write_mask != 0xff) {
      cache_.depth_stencil_state.stencil_write_mask = 0xff;
      glStencilMask(0xff);
    }
  }

  if (clear_mask != 0) {
    glClear(clear_mask);
  }
}


void OpenGLRenderer::ApplyViewPort(int32 x, int32 y, int32 width, int32 height) {
  if (cache_.viewport_x != x ||
      cache_.viewport_y != y ||
      cache_.viewport_width != width ||
      cache_.viewport_height != height) {
    cache_.viewport_x = x;
    cache_.viewport_y = y;
    cache_.viewport_width = width;
    cache_.viewport_height = height;
    glViewport(x, y, width, height);
  }
}

void OpenGLRenderer::ApplyScissor(int32 x, int32 y, int32 width, int32 height) {
  if (cache_.scissor_x != x ||
      cache_.scissor_y != y ||
      cache_.scissor_width != width ||
      cache_.scissor_height != height) {
    cache_.scissor_x = x;
    cache_.scissor_y = y;
    cache_.scissor_width = width;
    cache_.scissor_height = height;
    glScissor(x, y, width, height);
  }
}

void OpenGLRenderer::ApplyPipeline(ResourceID id) {
  auto &pipeline = resource_manager()->pipeline_pool_.Find(id);
  if (pipeline.status() == ResourceStatus::kCompleted) {
    ApplyShader(pipeline.config().shader);
    ApplyBlendState(pipeline.config().blend_state);
    ApplyDepthStencilState(pipeline.config().depth_stencil_state);
    ApplyRasterizerState(pipeline.config().rasterizer_state);
  }
}

void OpenGLRenderer::ResetPipeline() {
  ResetBlendState();
  ResetDepthStencilState();
  ResetRasterizerState();
}

void OpenGLRenderer::ApplyShader(ResourceID id) {
  auto &shader = resource_manager()->shader_pool_.Find(id);
  if (shader.status() == ResourceStatus::kCompleted) {
    if (shader.program_id != cache_.program_id) {
      glUseProgram(shader.program_id);
      cache_.program_id = shader.program_id;
    }
  }
}

void OpenGLRenderer::UpdateShaderResourceData(ResourceID shader_id, const eastl::string &name, DataPtr data) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    if (shader.program_id != cache_.program_id) {
      glUseProgram(shader.program_id);
      cache_.program_id = shader.program_id;
    }
    const auto &pair = shader.uniform_info.find(name);
    if (pair != shader.uniform_info.end()) {
      auto &info = pair->second;
      if (data != nullptr && data->size() >= info.size * SizeOfOpenGLType(info.type)) {
        switch (info.type) {
          case GL_FLOAT: {
            glUniform1fv(info.location, info.size, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_VEC2: {
            glUniform2fv(info.location, info.size, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_VEC3: {
            glUniform3fv(info.location, info.size, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_VEC4: {
            glUniform4fv(info.location, info.size, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_DOUBLE: {
            glUniform1dv(info.location, info.size, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_VEC2: {
            glUniform2dv(info.location, info.size, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_VEC3: {
            glUniform3dv(info.location, info.size, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_VEC4: {
            glUniform4dv(info.location, info.size, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_BOOL:
          case GL_INT: {
            glUniform1iv(info.location, info.size, reinterpret_cast<const GLint *>(data->buffer()));
            break;
          }
          case GL_BOOL_VEC2:
          case GL_INT_VEC2: {
            glUniform2iv(info.location, info.size, reinterpret_cast<const GLint *>(data->buffer()));
            break;
          }
          case GL_BOOL_VEC3:
          case GL_INT_VEC3: {
            glUniform3iv(info.location, info.size, reinterpret_cast<const GLint *>(data->buffer()));
            break;
          }
          case GL_BOOL_VEC4:
          case GL_INT_VEC4: {
            glUniform4iv(info.location, info.size, reinterpret_cast<const GLint *>(data->buffer()));
            break;
          }
          case GL_UNSIGNED_INT: {
            glUniform1uiv(info.location, info.size, reinterpret_cast<const GLuint *>(data->buffer()));
            break;
          }
          case GL_UNSIGNED_INT_VEC2: {
            glUniform2uiv(info.location, info.size, reinterpret_cast<const GLuint *>(data->buffer()));
            break;
          }
          case GL_UNSIGNED_INT_VEC3: {
            glUniform3uiv(info.location, info.size, reinterpret_cast<const GLuint *>(data->buffer()));
            break;
          }
          case GL_UNSIGNED_INT_VEC4: {
            glUniform4uiv(info.location, info.size, reinterpret_cast<const GLuint *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT2: {
            glUniformMatrix2fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT3: {
            glUniformMatrix3fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT4: {
            glUniformMatrix4fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT2x3: {
            glUniformMatrix2x3fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT2x4: {
            glUniformMatrix2x4fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT3x2: {
            glUniformMatrix3x2fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT3x4: {
            glUniformMatrix3x4fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT4x2: {
            glUniformMatrix4x2fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_FLOAT_MAT4x3: {
            glUniformMatrix4x3fv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLfloat *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT2: {
            glUniformMatrix2dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT3: {
            glUniformMatrix3dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT4: {
            glUniformMatrix4dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT2x3: {
            glUniformMatrix2x3dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT2x4: {
            glUniformMatrix2x4dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT3x2: {
            glUniformMatrix3x2dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT3x4: {
            glUniformMatrix3x4dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT4x2: {
            glUniformMatrix4x2dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          case GL_DOUBLE_MAT4x3: {
            glUniformMatrix4x3dv(info.location, info.size, GL_FALSE, reinterpret_cast<const GLdouble *>(data->buffer()));
            break;
          }
          default: {
            Log::GetInstance().Error("type of uniform %s should not be update with UpdateShaderResourceData\n", name.c_str());
          }
        }
      } else {
        Log::GetInstance().Error("buffer size is too small to fit into uniform: %s\n", name.c_str());
      }
    } else {
      Log::GetInstance().Error("cannot find uniform: %s\n", name.c_str());
    }
  }
}

void OpenGLRenderer::UpdateShaderResourceTexture(ResourceID shader_id, const eastl::string &name, ResourceID texture_id) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    if (shader.program_id != cache_.program_id) {
      glUseProgram(shader.program_id);
      cache_.program_id = shader.program_id;
    }
    const auto &pair = shader.uniform_info.find(name);
    if (pair != shader.uniform_info.end()) {
      auto &info = pair->second;
      if (info.type == GL_SAMPLER_2D) {
        ApplyTexture(texture_id, info.texture_2d_index);
      } else if (info.type == GL_SAMPLER_CUBE) {
        ApplyTexture(texture_id, info.texture_cube_index);
      } else {
        Log::GetInstance().Error("uniform %s should not be Texture\n", name.c_str());
      }
    } else {
      Log::GetInstance().Error("cannot find texture: %s\n", name.c_str());
    }
  }
}

void OpenGLRenderer::UpdateShaderResourceSampler(ResourceID shader_id, const eastl::string &name, ResourceID sampler_id) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    if (shader.program_id != cache_.program_id) {
      glUseProgram(shader.program_id);
      cache_.program_id = shader.program_id;
    }
    auto index = -1;
    const auto &pair = shader.uniform_info.find(name);
    if (pair != shader.uniform_info.end()) {
      auto &info = pair->second;
      if (info.type == GL_SAMPLER_2D) {
        index = info.texture_2d_index;
      } else if (info.type == GL_SAMPLER_CUBE) {
        index = info.texture_cube_index;
      } else {
        Log::GetInstance().Error("uniform %s should not be Texture\n", name.c_str());
      }
    } else {
      Log::GetInstance().Error("cannot find texture: %s\n", name.c_str());
    }
    if (index != -1) {
      ApplySampler(sampler_id, index);
    }
  }
}

void OpenGLRenderer::UpdateShaderResourceBlock(ResourceID shader_id, const eastl::string &name, ResourceID uniform_buffer_id) {
  auto &shader = resource_manager()->shader_pool_.Find(shader_id);
  if (shader.status() == ResourceStatus::kCompleted) {
    if (shader.program_id != cache_.program_id) {
      glUseProgram(shader.program_id);
      cache_.program_id = shader.program_id;
    }
    auto &uniform_buffer = resource_manager()->uniform_buffer_pool_.Find(uniform_buffer_id);
    if (uniform_buffer.status() == ResourceStatus::kCompleted) {
      const auto &pair = shader.uniform_block_info.find(name);
      if (pair != shader.uniform_block_info.end()) {
        if (cache_.uniform_buffer[pair->second.location] != uniform_buffer.uniform_buffer_id) {
          glBindBufferBase(GL_UNIFORM_BUFFER, pair->second.location, uniform_buffer.uniform_buffer_id);
          cache_.uniform_buffer[pair->second.location] = uniform_buffer.uniform_buffer_id;
        }
      } else {
        Log::GetInstance().Error("cannot find uniform block: %s\n", name.c_str());
      }
    }
  }
}

void OpenGLRenderer::ResetShader() {
  cache_.program_id = 0;
  glUseProgram(0);
}

void OpenGLRenderer::UpdateUniformBufferData(ResourceID id, DataPtr data) {
  auto &uniform_buffer = resource_manager()->uniform_buffer_pool_.Find(id);
  if (uniform_buffer.status() == ResourceStatus::kCompleted) {
    if (data == nullptr || uniform_buffer.config().size < data->size()) {
      Log::GetInstance().Error("uniform buffer size is smaller then offset + length\n");
    } else {
      GLint current_uniform_buffer;
      glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &current_uniform_buffer);

      glBindBuffer(GL_UNIFORM_BUFFER, uniform_buffer.uniform_buffer_id);

      auto source = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
      memcpy(source, data->buffer(), data->size());
      glUnmapBuffer(GL_UNIFORM_BUFFER);

      glBindBuffer(GL_UNIFORM_BUFFER, static_cast<GLuint>(current_uniform_buffer));
    }
  }
}

void OpenGLRenderer::ApplyMesh(ResourceID id) {
  auto &mesh = resource_manager()->mesh_pool_.Find(id);
  if (mesh.status() == ResourceStatus::kCompleted) {
    if (mesh.vertex_buffer_id != cache_.vertex_buffer) {
      glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
      cache_.vertex_buffer = mesh.vertex_buffer_id;
    }
    if (mesh.vertex_array_id != cache_.vertex_array) {
      glBindVertexArray(mesh.vertex_array_id);
      cache_.vertex_array = mesh.vertex_array_id;
    }
    if (mesh.config().index_type != cache_.index_type) {
      cache_.index_type = mesh.config().index_type;
    }
  }
}

void OpenGLRenderer::UpdateMesh(ResourceID id, DataPtr vertex_data, DataPtr index_data) {
  auto &mesh = resource_manager()->mesh_pool_.Find(id);
  if (mesh.status() == ResourceStatus::kCompleted) {
    if (mesh.vertex_buffer_id != cache_.vertex_buffer) {
      glBindBuffer(GL_ARRAY_BUFFER, mesh.vertex_buffer_id);
      cache_.vertex_buffer = mesh.vertex_buffer_id;
    }
    if (vertex_data != nullptr && vertex_data->size() != 0) {
      if (mesh.config().vertex_count * mesh.config().layout.size < vertex_data->size()) {
        Log::GetInstance().Error("UpdateMesh failed, vertex size: %d, but length: %d\n",
                                 mesh.config().vertex_count * mesh.config().layout.size, vertex_data->buffer());
      } else {
        auto vertex_source = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(vertex_source, vertex_data->buffer(), vertex_data->size());
        glUnmapBuffer(GL_ARRAY_BUFFER);
      }
    }
    if (mesh.vertex_array_id != cache_.vertex_array) {
      glBindVertexArray(mesh.vertex_array_id);
      cache_.vertex_array = mesh.vertex_array_id;
    }
    if (mesh.config().index_type != cache_.index_type) {
      cache_.index_type = mesh.config().index_type;
    }
    if (index_data != nullptr && index_data->size() != 0) {
      if (mesh.config().index_count * SizeOfIndexFormat(mesh.config().index_type) < index_data->size()) {
        Log::GetInstance().Error("UpdateMesh failed, index size: %d, but want length: %d\n",
                                 mesh.config().index_count * SizeOfIndexFormat(mesh.config().index_type), index_data->buffer());
      } else {
        auto index_source = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        memcpy(index_source, index_data->buffer(), index_data->size());
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
      }
    }
  }
}

void OpenGLRenderer::ResetMesh() {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  cache_.vertex_buffer = 0;
  cache_.index_type = IndexFormat::kNone;
}

void OpenGLRenderer::Draw(const DrawCallState &state) {
  if (cache_.program_id == 0) return;
  if (cache_.vertex_buffer == 0) return;
  if (cache_.vertex_array == 0) return;
  if (cache_.index_type == IndexFormat::kNone) {
    glDrawArrays(GLEnumForVertexTopology(state.topology),
                 static_cast<GLint>(state.first),
                 static_cast<GLint>(state.count));
  } else {
    glDrawElements(GLEnumForVertexTopology(state.topology),
                   static_cast<GLint>(state.count),
                   GLEnumForIndexFormat(cache_.index_type),
                   reinterpret_cast<GLvoid *>(state.first * SizeOfIndexFormat(cache_.index_type)));
  }
}

void OpenGLRenderer::Reset() {
  ResetShader();
  ResetTexture();
  ResetMesh();
  ResetPipeline();
  ResetSampler();
}

void OpenGLRenderer::ApplySampler(ResourceID id, int32 index) {
  auto &sampler = resource_manager()->sampler_pool_.Find(id);
  if (sampler.status() == ResourceStatus::kCompleted) {
    if (cache_.sampler_id[index] != sampler.sampler_id) {
      glBindSampler(static_cast<GLuint>(index), sampler.sampler_id);
      cache_.sampler_id[index] = sampler.sampler_id;
    }
  }
}

void OpenGLRenderer::ResetSampler() {
  memset(cache_.sampler_id, 0, sizeof(cache_.sampler_id));
  for (auto i = 0; i < static_cast<uint16>(GraphicsMaxDefine::kMaxSamplerCount); ++i) {
    glBindSampler(static_cast<GLuint>(i), 0);
  }
}

void OpenGLRenderer::ApplyTexture(ResourceID id, int32 index) {
  auto &texture = resource_manager()->texture_pool_.Find(id);
  if (texture.status() == ResourceStatus::kCompleted) {
    if (texture.config().type == TextureType::kTexture2d) {
      if (texture.texture_id != cache_.texture_2d[index]) {
        glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));
        glBindTexture(GL_TEXTURE_2D, texture.texture_id);
        cache_.texture_2d[index] = texture.texture_id;
      }
    } else if (texture.config().type == TextureType::kTextureCube) {
      if (texture.texture_id != cache_.texture_cube[index]) {
        glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture.texture_id);
        cache_.texture_cube[index] = texture.texture_id;
      }
    }
  }
}

void OpenGLRenderer::ResetTexture() {
  for (auto index = 0; index < static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount); ++index) {
    if (cache_.texture_2d[index] != 0) {
      glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));
      glBindTexture(GL_TEXTURE_2D, 0);
    }
    if (cache_.texture_cube[index] != 0) {
      glActiveTexture(static_cast<GLenum>(GL_TEXTURE0 + index));
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
  }
  memset(cache_.texture_2d, 0, sizeof(cache_.texture_2d));
  memset(cache_.texture_cube, 0, sizeof(cache_.texture_cube));
}

void OpenGLRenderer::ApplyBlendState(const BlendState &blend_state) {
  if (blend_state.blend_color != cache_.blend_color) {
    cache_.blend_color = blend_state.blend_color;
    glBlendColor(blend_state.blend_color.r,
                 blend_state.blend_color.g,
                 blend_state.blend_color.b,
                 blend_state.blend_color.a);
  }
  if (blend_state.enable != cache_.blend_state.enable) {
    if (blend_state.enable) {
      glEnable(GL_BLEND);
    } else {
      glDisable(GL_BLEND);
    }
  }
  if (blend_state.src_rgb_factor != cache_.blend_state.src_rgb_factor ||
      blend_state.dst_rgb_factor != cache_.blend_state.dst_rgb_factor ||
      blend_state.src_alpha_factor != cache_.blend_state.src_alpha_factor ||
      blend_state.dst_alpha_factor != cache_.blend_state.dst_alpha_factor) {
    glBlendFuncSeparate(GLEnumForBlendFactor(blend_state.src_rgb_factor),
                        GLEnumForBlendFactor(blend_state.dst_rgb_factor),
                        GLEnumForBlendFactor(blend_state.src_alpha_factor),
                        GLEnumForBlendFactor(blend_state.dst_alpha_factor));
  }
  if (blend_state.rgb_operation != cache_.blend_state.rgb_operation ||
      blend_state.alpha_operation != cache_.blend_state.alpha_operation) {
    glBlendEquationSeparate(GLEnumForBlendOperation(blend_state.rgb_operation),
                            GLEnumForBlendOperation(blend_state.alpha_operation));
  }
  if (blend_state.color_mask != cache_.blend_state.color_mask) {
    glColorMask(static_cast<uint8>(blend_state.color_mask) & static_cast<uint8>(PixelChannel::kRed),
                static_cast<uint8>(blend_state.color_mask) & static_cast<uint8>(PixelChannel::kGreen),
                static_cast<uint8>(blend_state.color_mask) & static_cast<uint8>(PixelChannel::kBlue),
                static_cast<uint8>(blend_state.color_mask) & static_cast<uint8>(PixelChannel::kAlpha));
  }
  cache_.blend_state = blend_state;
}

void OpenGLRenderer::ResetBlendState() {
  cache_.blend_state = BlendState();
  glDisable(GL_BLEND);
  glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ONE, GL_ZERO);
  glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  cache_.blend_color = Color(1.0f, 1.0f, 1.0f, 1.0f);
  glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void OpenGLRenderer::ApplyDepthStencilState(const DepthStencilState &depth_stencil_state) {
  if (cache_.depth_stencil_state != depth_stencil_state) {
    if (cache_.depth_stencil_state.value != depth_stencil_state.value) {
      if (depth_stencil_state.depth_compare != cache_.depth_stencil_state.depth_compare) {
        glDepthFunc(GLEnumForCompareFunction(depth_stencil_state.depth_compare));
      }
      if (depth_stencil_state.depth_enable != cache_.depth_stencil_state.depth_enable) {
        if (depth_stencil_state.depth_enable) {
          glEnable(GL_DEPTH_TEST);
        } else {
          glDisable(GL_DEPTH_TEST);
        }
      }
      if (depth_stencil_state.depth_write_enable != cache_.depth_stencil_state.depth_write_enable) {
        glDepthMask(static_cast<GLboolean>(depth_stencil_state.depth_write_enable));
      }
      if (depth_stencil_state.stencil_enable != cache_.depth_stencil_state.stencil_enable) {
        if (depth_stencil_state.stencil_enable) {
          glEnable(GL_STENCIL_TEST);
        } else {
          glDisable(GL_STENCIL_TEST);
        }
      }
      if (depth_stencil_state.stencil_write_mask != cache_.depth_stencil_state.stencil_write_mask) {
        glStencilMask(depth_stencil_state.stencil_write_mask);
        cache_.depth_stencil_state.stencil_write_mask = depth_stencil_state.stencil_write_mask;
      }
    }
    if (cache_.depth_stencil_state.front != depth_stencil_state.front) {
      if (depth_stencil_state.front.fail != cache_.depth_stencil_state.front.fail ||
          depth_stencil_state.front.depth_fail != cache_.depth_stencil_state.front.depth_fail ||
          depth_stencil_state.front.pass != cache_.depth_stencil_state.front.pass) {
        glStencilOpSeparate(GL_FRONT,
                            GLEnumForStencilOperation(depth_stencil_state.front.fail),
                            GLEnumForStencilOperation(depth_stencil_state.front.depth_fail),
                            GLEnumForStencilOperation(depth_stencil_state.front.pass));
      }
    }
    if (depth_stencil_state.stencil_read_mask != cache_.depth_stencil_state.stencil_read_mask ||
        depth_stencil_state.front.compare != cache_.depth_stencil_state.front.compare ||
        depth_stencil_state.stencil_value != cache_.depth_stencil_state.stencil_value) {
      glStencilFuncSeparate(GL_FRONT,
                            GLEnumForCompareFunction(depth_stencil_state.front.compare),
                            depth_stencil_state.stencil_value,
                            depth_stencil_state.stencil_read_mask);
    }
    if (cache_.depth_stencil_state.back != depth_stencil_state.back) {
      if (depth_stencil_state.back.fail != cache_.depth_stencil_state.back.fail ||
          depth_stencil_state.back.depth_fail != cache_.depth_stencil_state.back.depth_fail ||
          depth_stencil_state.back.pass != cache_.depth_stencil_state.back.pass) {
        glStencilOpSeparate(GL_BACK,
                            GLEnumForStencilOperation(depth_stencil_state.back.fail),
                            GLEnumForStencilOperation(depth_stencil_state.back.depth_fail),
                            GLEnumForStencilOperation(depth_stencil_state.back.pass));
      }
    }
    if (depth_stencil_state.stencil_read_mask != cache_.depth_stencil_state.stencil_read_mask ||
        depth_stencil_state.back.compare != cache_.depth_stencil_state.back.compare ||
        depth_stencil_state.stencil_value != cache_.depth_stencil_state.stencil_value) {
      glStencilFuncSeparate(GL_BACK,
                            GLEnumForCompareFunction(depth_stencil_state.back.compare),
                            depth_stencil_state.stencil_value,
                            depth_stencil_state.stencil_read_mask);
    }
    cache_.depth_stencil_state = depth_stencil_state;
  }
}

void OpenGLRenderer::ResetDepthStencilState() {
  cache_.depth_stencil_state = DepthStencilState();
  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_FALSE);
  glDisable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0, 0xFFFFFFFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glStencilMask(0xFFFFFFFF);
}

void OpenGLRenderer::ApplyRasterizerState(const RasterizerState &rasterizer_state) {
  if (cache_.rasterizer_state != rasterizer_state) {
    if (rasterizer_state.cull_face_enable) {
      glEnable(GL_CULL_FACE);
    } else {
      glDisable(GL_CULL_FACE);
    }
    if (rasterizer_state.depth_offset_enable) {
      glEnable(GL_POLYGON_OFFSET_FILL);
    } else {
      glDisable(GL_POLYGON_OFFSET_FILL);
    }
    if (rasterizer_state.scissor_test_enable) {
      glEnable(GL_SCISSOR_TEST);
    } else {
      glDisable(GL_SCISSOR_TEST);
    }
    if (rasterizer_state.dither_enable) {
      glEnable(GL_DITHER);
    } else {
      glDisable(GL_DITHER);
    }
    if (rasterizer_state.alpha_to_coverage_enable) {
      glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    } else {
      glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
    }
    if (rasterizer_state.sample > 1) {
      glEnable(GL_MULTISAMPLE);
    } else {
      glDisable(GL_MULTISAMPLE);
    }

    if (rasterizer_state.fill_mode != cache_.rasterizer_state.fill_mode) {
      glPolygonMode(GL_FRONT_AND_BACK, rasterizer_state.fill_mode == FillMode::kWireframe ? GL_LINE : GL_FILL);
    }

    if (rasterizer_state.cull_face != cache_.rasterizer_state.cull_face) {
      if (rasterizer_state.cull_face_enable) {
        glCullFace(GLEnumForFaceSide(rasterizer_state.cull_face));
      }
    }

    if (rasterizer_state.front_face != cache_.rasterizer_state.front_face) {
      glFrontFace(GLEnumForFrontFaceType(rasterizer_state.front_face));
    }

    cache_.rasterizer_state = rasterizer_state;
  }
}

void OpenGLRenderer::ResetRasterizerState() {
  cache_.rasterizer_state = RasterizerState();
  glDisable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  glDisable(GL_POLYGON_OFFSET_FILL);
  glDisable(GL_SCISSOR_TEST);
  glEnable(GL_DITHER);
  glEnable(GL_MULTISAMPLE);
}

} // namespace xEngine

#endif // X_OPENGL
