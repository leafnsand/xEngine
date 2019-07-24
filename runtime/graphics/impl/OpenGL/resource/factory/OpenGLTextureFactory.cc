#if X_OPENGL

#include "OpenGLTextureFactory.h"

#include "graphics/impl/OpenGL/OpenGLDefine.h"

#include "core/Log.h"

namespace xEngine {

void OpenGLTextureFactory::Create(OpenGLTexture &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  auto &config = resource.config();

  auto face_count = config.type == TextureType::kTextureCube ?
                      static_cast<uint16>(GraphicsMaxDefine::kMaxCubeTextureFaceCount) :
                      static_cast<uint16>(1);
  auto is_compressed = IsCompressedPixelFormat(config.color_format);
  auto internal_format = GLEnumForPixelFormatAsInternal(config.color_format);
  auto layout = is_compressed ? 0 : GLEnumForPixelFormatAsLayout(config.color_format);
  auto format = GLEnumForPixelFormatAsFormat(config.color_format);

  for (auto i = 0; i < face_count; ++i) {
    for (auto j = 0; j < config.mipmap_count; ++j) {
      auto width = config.width >> j;
      auto height = config.height >> j;
      if (width == 0) width = 1;
      if (height == 0) height = 1;
      if (config.data[i][j] == nullptr ||
          config.data[i][j]->size() < SizeOfPixelFormat(config.color_format) * width * height) {
        Log::GetInstance().Error("[OpenGLTextureFactory::Create] wrong data size\n");
        resource.Failed();
        return;
      }
    }
  }

  auto target = GLEnumForTextureType(config.type);

  GLint current_texture;
  glGetIntegerv(GLEnumForTextureTypeAsBinding(config.type), &current_texture);

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  x_assert(texture_id != 0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(target, texture_id);

  for (auto face_index = 0; face_index < face_count; ++face_index) {
    auto real_target = target;
    if (config.type == TextureType::kTextureCube) {
      switch (face_index) {
        case 0:
          real_target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
          break;
        case 1:
          real_target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
          break;
        case 2:
          real_target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
          break;
        case 3:
          real_target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
          break;
        case 4:
          real_target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
          break;
        default:
          real_target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
          break;
      }
    }
    for (auto mipmap_index = 0; mipmap_index < config.mipmap_count; ++mipmap_index) {
      auto width = config.width >> mipmap_index;
      auto height = config.height >> mipmap_index;
      if (width == 0) width = 1;
      if (height == 0) height = 1;
      if (is_compressed) {
        glCompressedTexImage2D(real_target,
                               mipmap_index,
                               internal_format,
                               width,
                               height,
                               0,
                               static_cast<GLsizei>(config.data[face_index][mipmap_index]->size()),
                               config.data[face_index][mipmap_index]->buffer());
      } else {
        glTexImage2D(real_target,
                     mipmap_index,
                     internal_format,
                     width,
                     height,
                     0,
                     format,
                     layout,
                     config.data[face_index][mipmap_index]->buffer());
      }
    }
  }

  for (auto i = 0; i < static_cast<uint16>(GraphicsMaxDefine::kMaxCubeTextureFaceCount); ++i) {
    for (auto j = 0; j < static_cast<uint16>(GraphicsMaxDefine::kMaxTextureMipMapCount); ++j) {
      if (resource.config().data[i][j] != nullptr) {
        resource.config().data[i][j].reset();
      }
    }
  }

  resource.texture_id = texture_id;

  resource.Complete();
}

void OpenGLTextureFactory::Destroy(OpenGLTexture &resource) {
  x_assert(resource.status() == ResourceStatus::kCompleted || resource.status() == ResourceStatus::kFailed);
  if (resource.texture_id != 0) {
    glDeleteTextures(1, &resource.texture_id);
  }
}

} // namespace xEngine

#endif // X_OPENGL