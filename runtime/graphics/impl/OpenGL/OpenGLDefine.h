#ifndef XENGINE_GRAPHICS_IMPL_OPENGL_OPENGLDEFINE_H
#define XENGINE_GRAPHICS_IMPL_OPENGL_OPENGLDEFINE_H

#if X_OPENGL

#include "graphics/GraphicsDefine.h"
#include "graphics/config/GraphicsConfig.h"
#include "graphics/state/BlendState.h"
#include "graphics/state/DepthStencilState.h"
#include "graphics/state/RasterizerState.h"

#include "graphics/impl/OpenGL/resource/OpenGLGraphicsResource.h"

#include "window/WindowDefine.h"

#include <glad/glad.h>

namespace xEngine {

struct OpenGLRendererCache {
  BlendState blend_state;
  DepthStencilState depth_stencil_state;
  RasterizerState rasterizer_state;

  GLint viewport_x{0};
  GLint viewport_y{0};
  GLsizei viewport_width{0};
  GLsizei viewport_height{0};

  GLint scissor_x{0};
  GLint scissor_y{0};
  GLsizei scissor_width{0};
  GLsizei scissor_height{0};

  Color blend_color;

  GLuint frame_buffer{0};

  GLuint program_id{0};

  GLuint vertex_buffer{0};
  GLuint vertex_array{0};
  IndexFormat index_type{IndexFormat::kNone};

  GLuint uniform_buffer[static_cast<uint16>(GraphicsMaxDefine::kMaxUniformBlockCount)];

  GLuint texture_2d[static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount)];
  GLuint texture_cube[static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount)];

  GLuint sampler_id[static_cast<uint16>(GraphicsMaxDefine::kMaxSamplerCount)];

  OpenGLRendererCache() {
    memset(uniform_buffer, 0, sizeof(uniform_buffer));
    memset(texture_2d, 0, sizeof(texture_2d));
    memset(texture_cube, 0, sizeof(texture_cube));
    memset(sampler_id, 0, sizeof(sampler_id));
  }
};

static GLenum GLEnumForBufferUsage(BufferUsage usage) {
  switch (usage) {
    case BufferUsage::kImmutable:
      return GL_STATIC_DRAW;
    case BufferUsage::kDynamic:
      return GL_DYNAMIC_DRAW;
    case BufferUsage::kStream:
      return GL_STREAM_DRAW;
    default:
      x_error("unknown BufferUsage\n");
      return 0;
  }
}

static GLenum GLEnumForTextureFilterMode(TextureFilterMode mode) {
  switch (mode) {
    case TextureFilterMode::kNearest:
      return GL_NEAREST;
    case TextureFilterMode::kLinear:
      return GL_LINEAR;
    case TextureFilterMode::kNearestMipmapNearest:
      return GL_NEAREST_MIPMAP_NEAREST;
    case TextureFilterMode::kNearestMipmapLinear:
      return GL_NEAREST_MIPMAP_LINEAR;
    case TextureFilterMode::kLinearMipmapNearest:
      return GL_LINEAR_MIPMAP_NEAREST;
    case TextureFilterMode::kLinearMipmapLinear:
      return GL_LINEAR_MIPMAP_LINEAR;
    default:
      x_error("unknown TextureFilterMode\n");
      return 0;
  }
}

static GLenum GLEnumForTextureWrapMode(TextureWrapMode mode) {
  switch (mode) {
    case TextureWrapMode::kClampToEdge:
      return GL_CLAMP_TO_EDGE;
    case TextureWrapMode::kRepeat:
      return GL_REPEAT;
    case TextureWrapMode::kMirroredRepeat:
      return GL_MIRRORED_REPEAT;
    default:
      x_error("unknown TextureWrapMode\n");
      return 0;
  }
}

static GLenum GLEnumForTextureType(TextureType type) {
  switch (type) {
    case TextureType::kTexture2d:
      return GL_TEXTURE_2D;
    case TextureType::kTexture3d:
      return GL_TEXTURE_3D;
    case TextureType::kTextureCube:
      return GL_TEXTURE_CUBE_MAP;
    default:
      x_error("unknown TextureType\n");
      return 0;
  }
}

static GLenum GLEnumForTextureTypeAsBinding(TextureType type) {
  switch (type) {
    case TextureType::kTexture2d:
      return GL_TEXTURE_BINDING_2D;
    case TextureType::kTexture3d:
      return GL_TEXTURE_BINDING_3D;
    case TextureType::kTextureCube:
      return GL_TEXTURE_BINDING_CUBE_MAP;
    default:
      x_error("unknown TextureType\n");
      return 0;
  }
}

static GLenum GLEnumForPixelFormatAsInternal(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA8:
      return GL_RGBA8;
    case PixelFormat::RGB8:
      return GL_RGB8;
    case PixelFormat::RGBA4:
      return GL_RGBA4;
    case PixelFormat::R5G6B5:
      return GL_RGB5;
    case PixelFormat::R5G5B5A1:
      return GL_RGB5_A1;
    case PixelFormat::RGBA32F:
      return GL_RGBA32F;
    case PixelFormat::RGBA16F:
      return GL_RGBA16F;
    case PixelFormat::L8:
      return GL_R8;
    case PixelFormat::DXT1:
      return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case PixelFormat::DXT3:
      return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat::DXT5:
      return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat::D16:
      return GL_DEPTH_COMPONENT16;
    case PixelFormat::D32:
      return GL_DEPTH_COMPONENT32F;
    case PixelFormat::D24S8:
      return GL_DEPTH24_STENCIL8;
    case PixelFormat::PVRTC2_RGB:
      return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    case PixelFormat::PVRTC4_RGB:
      return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    case PixelFormat::PVRTC2_RGBA:
      return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case PixelFormat::PVRTC4_RGBA:
      return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    case PixelFormat::ETC2_RGB8:
      return GL_COMPRESSED_RGB8_ETC2;
    case PixelFormat::ETC2_SRGB8:
      return GL_COMPRESSED_SRGB8_ETC2;
    default:
      x_error("unknown PixelFormat\n");
      return 0;
  }
}

static GLenum GLEnumForPixelFormatAsLayout(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA32F:
      return GL_FLOAT;
    case PixelFormat::RGBA16F:
      return GL_HALF_FLOAT;
    case PixelFormat::RGBA8:
    case PixelFormat::RGB8:
    case PixelFormat::L8:
      return GL_UNSIGNED_BYTE;
    case PixelFormat::R5G5B5A1:
      return GL_UNSIGNED_SHORT_5_5_5_1;
    case PixelFormat::R5G6B5:
      return GL_UNSIGNED_SHORT_5_6_5;
    case PixelFormat::RGBA4:
      return GL_UNSIGNED_SHORT_4_4_4_4;
    case PixelFormat::D16:
      return GL_UNSIGNED_SHORT;
    case PixelFormat::D32:
      return GL_UNSIGNED_INT;
    case PixelFormat::D24S8:
      return GL_UNSIGNED_INT_24_8;
    default:
      x_error("unknown PixelFormat\n");
      return 0;
  }
}

static GLenum GLEnumForPixelFormatAsFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA8:
    case PixelFormat::R5G5B5A1:
    case PixelFormat::RGBA4:
    case PixelFormat::RGBA32F:
    case PixelFormat::RGBA16F:
      return GL_RGBA;
    case PixelFormat::RGB8:
    case PixelFormat::R5G6B5:
      return GL_RGB;
    case PixelFormat::L8:
      return GL_RED;
    case PixelFormat::D16:
    case PixelFormat::D32:
      return GL_DEPTH_COMPONENT;
    case PixelFormat::D24S8:
      return GL_DEPTH_STENCIL;
    case PixelFormat::DXT1:
      return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
    case PixelFormat::DXT3:
      return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat::DXT5:
      return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat::PVRTC2_RGB:
      return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    case PixelFormat::PVRTC4_RGB:
      return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
    case PixelFormat::PVRTC2_RGBA:
      return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case PixelFormat::PVRTC4_RGBA:
      return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    case PixelFormat::ETC2_RGB8:
      return GL_COMPRESSED_RGB8_ETC2;
    case PixelFormat::ETC2_SRGB8:
      return GL_COMPRESSED_SRGB8_ETC2;
    default:
      x_error("unknown PixelFormat\n");
      return 0;
  }
}

static GLint VertexCountForVertexElementFormat(VertexElementFormat format) {
  switch (format) {
    case VertexElementFormat::kFloat1:
      return 1;
    case VertexElementFormat::kFloat2:
      return 2;
    case VertexElementFormat::kFloat3:
      return 3;
    case VertexElementFormat::kFloat4:
      return 4;
    case VertexElementFormat::kByte4:
      return 4;
    case VertexElementFormat::kByte4Normalized:
      return 4;
    case VertexElementFormat::kUnsignedByte4:
      return 4;
    case VertexElementFormat::kUnsignedByte4Normalized:
      return 4;
    case VertexElementFormat::kShort2:
      return 2;
    case VertexElementFormat::kShort2Normalized:
      return 2;
    case VertexElementFormat::kShort4:
      return 4;
    case VertexElementFormat::kShort4Normalized:
      return 4;
    default:
      x_error("unknown VertexElementFormat\n");
      return 0;
  }
}

static GLenum GLEnumForVertexElementFormat(VertexElementFormat format) {
  switch (format) {
    case VertexElementFormat::kFloat1:
      return GL_FLOAT;
    case VertexElementFormat::kFloat2:
      return GL_FLOAT;
    case VertexElementFormat::kFloat3:
      return GL_FLOAT;
    case VertexElementFormat::kFloat4:
      return GL_FLOAT;
    case VertexElementFormat::kByte4:
      return GL_BYTE;
    case VertexElementFormat::kByte4Normalized:
      return GL_BYTE;
    case VertexElementFormat::kUnsignedByte4:
      return GL_UNSIGNED_BYTE;
    case VertexElementFormat::kUnsignedByte4Normalized:
      return GL_UNSIGNED_BYTE;
    case VertexElementFormat::kShort2:
      return GL_SHORT;
    case VertexElementFormat::kShort2Normalized:
      return GL_SHORT;
    case VertexElementFormat::kShort4:
      return GL_SHORT;
    case VertexElementFormat::kShort4Normalized:
      return GL_SHORT;
    default:
      x_error("unknown VertexElementFormat\n");
      return 0;
  }
}

static GLenum GLEnumForVertexTopology(VertexTopology type) {
  switch (type) {
    case VertexTopology::kPoints:
      return GL_POINTS;
    case VertexTopology::kLines:
      return GL_LINES;
    case VertexTopology::kLineStrip:
      return GL_LINE_STRIP;
    case VertexTopology::kLineLoop:
      return GL_LINE_LOOP;
    case VertexTopology::kTriangles:
      return GL_TRIANGLES;
    case VertexTopology::kTriangleStrip:
      return GL_TRIANGLE_STRIP;
    case VertexTopology::kTriangleFan:
      return GL_TRIANGLE_FAN;
    default:
      x_error("unknown VertexTopology\n");
      return 0;
  }
}

static GLenum GLEnumForIndexFormat(IndexFormat type) {
  switch (type) {
    case IndexFormat::kUint16:
      return GL_UNSIGNED_SHORT;
    case IndexFormat::kUint32:
      return GL_UNSIGNED_INT;
    default:
      x_error("unknown IndexFormat\n");
      return 0;
  }
}

static GLenum GLEnumForBlendFactor(BlendFactor type) {
  switch (type) {
    case BlendFactor::kZero:
      return GL_ZERO;
    case BlendFactor::kOne:
      return GL_ONE;
    case BlendFactor::kSrcColor:
      return GL_SRC_COLOR;
    case BlendFactor::kOneMinusSrcColor:
      return GL_ONE_MINUS_SRC_COLOR;
    case BlendFactor::kSrcAlpha:
      return GL_SRC_ALPHA;
    case BlendFactor::kOneMinusSrcAlpha:
      return GL_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::kDstColor:
      return GL_DST_COLOR;
    case BlendFactor::kOneMinusDstColor:
      return GL_ONE_MINUS_DST_COLOR;
    case BlendFactor::kDstAlpha:
      return GL_DST_ALPHA;
    case BlendFactor::kOneMinusDstAlpha:
      return GL_ONE_MINUS_DST_ALPHA;
    case BlendFactor::kBlendColor:
      return GL_CONSTANT_COLOR;
    case BlendFactor::kOneMinusBlendColor:
      return GL_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::kBlendAlpha:
      return GL_CONSTANT_ALPHA;
    case BlendFactor::kOneMinusBlendAlpha:
      return GL_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::kSrcAlphaSaturated:
      return GL_SRC_ALPHA_SATURATE;
    default:
      x_error("unknown BlendFactor\n");
      return 0;
  }
}

static GLenum GLEnumForBlendOperation(BlendOperation type) {
  switch (type) {
    case BlendOperation::kAdd:
      return GL_FUNC_ADD;
    case BlendOperation::kSubtract:
      return GL_FUNC_SUBTRACT;
    case BlendOperation::kReverseSubtract:
      return GL_FUNC_REVERSE_SUBTRACT;
    default:
      x_error("unknown BlendOperation\n");
      return 0;
  }
}

static GLenum GLEnumForCompareFunction(CompareFunction func) {
  switch (func) {
    case CompareFunction::kNever:
      return GL_NEVER;
    case CompareFunction::kLess:
      return GL_LESS;
    case CompareFunction::kLessEqual:
      return GL_LEQUAL;
    case CompareFunction::kGreater:
      return GL_GREATER;
    case CompareFunction::kGreaterEqual:
      return GL_GEQUAL;
    case CompareFunction::kEqual:
      return GL_EQUAL;
    case CompareFunction::kNotEqual:
      return GL_NOTEQUAL;
    case CompareFunction::kAlways:
      return GL_ALWAYS;
    default:
      x_error("unknown CompareFunction\n");
      return 0;
  }
}

static GLenum GLEnumForStencilOperation(StencilOperation type) {
  switch (type) {
    case StencilOperation::kKeep:
      return GL_KEEP;
    case StencilOperation::kZero:
      return GL_ZERO;
    case StencilOperation::kReplace:
      return GL_REPLACE;
    case StencilOperation::kIncrement:
      return GL_INCR;
    case StencilOperation::kIncrementWrap:
      return GL_INCR_WRAP;
    case StencilOperation::kDecrement:
      return GL_DECR;
    case StencilOperation::kDecrementWrap:
      return GL_DECR_WRAP;
    case StencilOperation::kInvert:
      return GL_INVERT;
    default:
      x_error("unknown StencilOperation\n");
      return 0;
  }
}

static GLenum GLEnumForFaceSide(FaceSide face) {
  switch (face) {
    case FaceSide::kFront:
      return GL_FRONT;
    case FaceSide::kBack:
      return GL_BACK;
    case FaceSide::kBoth:
      return GL_FRONT_AND_BACK;
    default:
      x_error("unknown FaceSide\n");
      return 0;
  }
}

static GLenum GLEnumForFrontFaceType(FrontFaceType type) {
  switch (type) {
    case FrontFaceType::kClockWise:
      return GL_CW;
    case FrontFaceType::kCounterClockWise:
      return GL_CCW;
    default:
      x_error("unknown FaceSide\n");
      return 0;
  }
}

static const char *AttributeNameForVertexElementSemantic(VertexElementSemantic semantic) {
  switch (semantic) {
    case VertexElementSemantic::kPosition: return "aPosition";
    case VertexElementSemantic::kTexcoord0: return "aTexcoord0";
    case VertexElementSemantic::kTexcoord1: return "aTexcoord1";
    case VertexElementSemantic::kTexcoord2: return "aTexcoord2";
    case VertexElementSemantic::kTexcoord3: return "aTexcoord3";
    case VertexElementSemantic::kColor0: return "aColor0";
    case VertexElementSemantic::kColor1: return "aColor1";
    case VertexElementSemantic::kNormal: return "aNormal";
    case VertexElementSemantic::kTangent: return "aTangent";
    case VertexElementSemantic::kBinormal: return "aBinormal";
    case VertexElementSemantic::kWeights: return "aWeights";
    case VertexElementSemantic::kIndices: return "aIndices";
    case VertexElementSemantic::kInstance0: return "aInstance0";
    case VertexElementSemantic::kInstance1: return "aInstance1";
    case VertexElementSemantic::kInstance2: return "aInstance2";
    case VertexElementSemantic::kInstance3: return "aInstance3";
    default: x_error("unknown VertexElementSemantic\n"); return nullptr;
  }
}

static size_t SizeOfOpenGLType(GLenum type) {
  switch (type) {
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_BOOL:
    case GL_FLOAT: return 4;
    case GL_INT_VEC2:
    case GL_BOOL_VEC2:
    case GL_UNSIGNED_INT_VEC2:
    case GL_FLOAT_VEC2: return 8;
    case GL_INT_VEC3:
    case GL_UNSIGNED_INT_VEC3:
    case GL_BOOL_VEC3:
    case GL_FLOAT_VEC3: return 12;
    case GL_INT_VEC4:
    case GL_UNSIGNED_INT_VEC4:
    case GL_BOOL_VEC4:
    case GL_FLOAT_VEC4: return 16;
    case GL_DOUBLE: return 8;
    case GL_DOUBLE_VEC2: return 16;
    case GL_DOUBLE_VEC3: return 24;
    case GL_DOUBLE_VEC4: return 32;
    case GL_FLOAT_MAT2: return 16;
    case GL_FLOAT_MAT3: return 36;
    case GL_FLOAT_MAT4: return 64;
    case GL_FLOAT_MAT2x3: return 24;
    case GL_FLOAT_MAT2x4: return 32;
    case GL_FLOAT_MAT3x2: return 24;
    case GL_FLOAT_MAT3x4: return 48;
    case GL_FLOAT_MAT4x2: return 32;
    case GL_FLOAT_MAT4x3: return 48;
    case GL_DOUBLE_MAT2: return 32;
    case GL_DOUBLE_MAT3: return 72;
    case GL_DOUBLE_MAT4: return 128;
    case GL_DOUBLE_MAT2x3: return 48;
    case GL_DOUBLE_MAT2x4: return 64;
    case GL_DOUBLE_MAT3x2: return 48;
    case GL_DOUBLE_MAT3x4: return 96;
    case GL_DOUBLE_MAT4x2: return 64;
    case GL_DOUBLE_MAT4x3: return 96;
    default: return 0;
  }
}

}

#endif // X_OPENGL

#endif // XENGINE_GRAPHICS_IMPL_OPENGL_OPENGLDEFINE_H