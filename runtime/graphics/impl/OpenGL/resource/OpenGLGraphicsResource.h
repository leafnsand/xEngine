#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_OPENGLGRAPHICSRESOURCE_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_OPENGLGRAPHICSRESOURCE_H

#if X_OPENGL

#include "graphics/config/ShaderConfig.h"
#include "graphics/config/TextureConfig.h"
#include "graphics/config/MeshConfig.h"
#include "graphics/config/PipelineConfig.h"
#include "graphics/config/SamplerConfig.h"
#include "graphics/config/UniformBufferConfig.h"

#include "resource/Resource.h"

#include <glad/glad.h>

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

namespace xEngine {

struct OpenGLShader: public Resource<ShaderConfig> {

  struct UniformInfo {
    GLuint location{0};
    GLenum type{GL_INVALID_ENUM};
    GLsizei size{0};
    uint8 texture_2d_index{0};
    uint8 texture_cube_index{0};
  };

  struct UniformBlockInfo {
    GLuint location{0};
    GLsizei size{0};
  };

  GLuint program_id{0};

  eastl::hash_map<eastl::string, UniformInfo> uniform_info;

  eastl::hash_map<eastl::string, UniformBlockInfo> uniform_block_info;

  virtual void Reset() override {
    program_id = 0;
    uniform_info.clear();
    uniform_block_info.clear();
  }

};

struct OpenGLTexture: public Resource<TextureConfig> {

  GLuint texture_id{0};

  virtual void Reset() override {
    texture_id = 0;
  }

};

struct OpenGLMesh: public Resource<MeshConfig> {

  GLuint index_buffer_id{0};

  GLuint vertex_buffer_id{0};

  GLuint vertex_array_id{0};

  virtual void Reset() override {
    index_buffer_id = 0;
    vertex_buffer_id = 0;
    vertex_array_id = 0;
  }

};

struct OpenGLPipeline: public Resource<PipelineConfig> {

  virtual void Reset() override {

  }

};

struct OpenGLSampler: public Resource<SamplerConfig> {

  GLuint sampler_id{0};

  virtual void Reset() override {
    sampler_id = 0;
  }

};

struct OpenGLUniformBuffer: public Resource<UniformBufferConfig> {

  GLuint uniform_buffer_id{0};

  virtual void Reset() override {
    uniform_buffer_id = 0;
  }

};

}

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_RESOURCE_OPENGLGRAPHICSRESOURCE_H