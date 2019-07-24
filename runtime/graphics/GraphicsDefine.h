#ifndef XENGINE_GRAPHICS_GRAPHICSDEFINE_H
#define XENGINE_GRAPHICS_GRAPHICSDEFINE_H

#include "core/Core.h"
#include "core/Types.h"
#include "resource/Resource.h"

#include <glm.hpp>
#include <gtc/type_ptr.hpp>

namespace xEngine {

using Color = glm::vec4;

using IntColor = glm::highp_ivec4;

enum class GraphicsMaxDefine: uint16 {
  kMaxTextureCount = 16,
  kMaxVertexElementCount = 16, // OpenGL ES 3.0
  kMaxCubeTextureFaceCount = 6,
  kMaxTextureMipMapCount = 12,
  kMaxUniformBlockCount = 16,
  kMaxSamplerCount = 48,
};

enum class GraphicsType: uint8 {
  kOpenGL3,
  kD3D11,

  kCount,
};

enum class PixelChannel: uint8 {
  kNone = 0,

  kAlpha = 1 << 0,
  kRed = 1 << 1,
  kGreen = 1 << 2,
  kBlue = 1 << 3,
  kDepth = 1 << 4,
  kStencil = 1 << 5,

  kDepthStentil = static_cast<uint8>(kDepth) | static_cast<uint8>(kStencil),

  kRGB = static_cast<uint8>(kRed) | static_cast<uint8>(kGreen) | static_cast<uint8>(kBlue),

  kRGBA = static_cast<uint8>(kRed) | static_cast<uint8>(kGreen) | static_cast<uint8>(kBlue) | static_cast<uint8>(kAlpha),

  kAll = static_cast<uint8>(kRGBA) | static_cast<uint8>(kDepthStentil),
};

enum class PixelFormat: uint8 {
  NONE,
  RGBA8,          ///< 32-bit wide, 4 channels @ 8-bit
  RGB8,           ///< 24-bit wide, 3 channels @ 8-bit
  RGBA4,          ///< 16-bit wide, 4 channels @ 4-bit
  R5G6B5,         ///< 16-bit wide, 3 channels @ 5/6/5 bits
  R5G5B5A1,       ///< 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
  RGBA32F,        ///< 128-bit wide, 4 channel @ 32-bit float
  RGBA16F,        ///< 64-bit wide, 4 channel @ 16-bit float
  L8,             ///< 8-bit wide, single channel
  DXT1,           ///< DXT1 compressed format
  DXT3,           ///< DXT3 compressed format
  DXT5,           ///< DXT5 compressed format
  D16,            ///< 16-bit depth
  D32,            ///< 32-bit depth
  D24S8,          ///< 24-bit depth, 8-bit stencil
  PVRTC2_RGB,     ///< PVRTC2 compressed format (RGB)
  PVRTC4_RGB,     ///< PVRTC4 compressed format (RGB)
  PVRTC2_RGBA,    ///< PVRTC2 compressed format (RGBA)
  PVRTC4_RGBA,    ///< PVRTC4 compressed format (RGBA)
  ETC2_RGB8,      ///< ETC2 compressed format (RGB8)
  ETC2_SRGB8,     ///< ETC2 compressed format (SRGB8)
};

static bool IsDepthPixelFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::D16:
    case PixelFormat::D32:
    case PixelFormat::D24S8:
      return true;
    default:
      return false;
  }
}

static bool IsCompressedPixelFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::DXT1:
    case PixelFormat::DXT3:
    case PixelFormat::DXT5:
    case PixelFormat::PVRTC2_RGB:
    case PixelFormat::PVRTC4_RGB:
    case PixelFormat::PVRTC2_RGBA:
    case PixelFormat::PVRTC4_RGBA:
    case PixelFormat::ETC2_RGB8:
    case PixelFormat::ETC2_SRGB8:
      return true;
    default:
      return false;
  }
}

static size_t SizeOfPixelFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA8:
      return 4;
    case PixelFormat::RGB8:
      return 3;
    case PixelFormat::RGBA4:
      return 2;
    case PixelFormat::R5G6B5:
      return 2;
    case PixelFormat::R5G5B5A1:
      return 2;
    case PixelFormat::RGBA32F:
      return 16;
    case PixelFormat::RGBA16F:
      return 8;
    case PixelFormat::L8:
      return 1;
    default:
      return 0;
  }
}

static int8 ChannelBitOfPixelFormat(PixelFormat format, PixelChannel channel) {
  int8 count = 0;
  switch (format) {
    case PixelFormat::RGBA32F:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kGreen == channel) || (PixelChannel::kBlue == channel)
          || (PixelChannel::kAlpha == channel)) {
        count = 32;
      }
      break;
    case PixelFormat::RGBA16F:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kGreen == channel) || (PixelChannel::kBlue == channel)
          || (PixelChannel::kAlpha == channel)) {
        count = 16;
      }
      break;
    case PixelFormat::RGBA8:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kGreen == channel) || (PixelChannel::kBlue == channel)
          || (PixelChannel::kAlpha == channel)) {
        count = 8;
      }
      break;
    case PixelFormat::RGB8:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kGreen == channel) || (PixelChannel::kBlue == channel)) {
        count = 8;
      }
      break;
    case PixelFormat::R5G6B5:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kBlue == channel)) {
        count = 5;
      } else if (PixelChannel::kGreen == channel) {
        count = 6;
      }
      break;
    case PixelFormat::R5G5B5A1:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kGreen == channel) || (PixelChannel::kBlue == channel)) {
        count = 5;
      } else if (PixelChannel::kAlpha == channel) {
        count = 1;
      }
      break;
    case PixelFormat::RGBA4:
      if ((PixelChannel::kRed == channel) || (PixelChannel::kGreen == channel) || (PixelChannel::kBlue == channel)
          || (PixelChannel::kAlpha == channel)) {
        count = 4;
      }
      break;
    case PixelFormat::L8:
      if (PixelChannel::kRed == channel) {
        count = 8;
      }
      break;
    case PixelFormat::D16:
      if (PixelChannel::kDepth == channel) {
        count = 16;
      }
      break;
    case PixelFormat::D32:
      if (PixelChannel::kDepth == channel) {
        count = 32;
      }
      break;
    case PixelFormat::D24S8:
      if (PixelChannel::kDepth == channel) {
        count = 24;
      } else if (PixelChannel::kStencil == channel) {
        count = 8;
      }
      break;
    default:
      break;
  }
  return count;
}

enum class BlendFactor: uint8 {
  kZero,
  kOne,

  kSrcColor,
  kOneMinusSrcColor,

  kSrcAlpha,
  kOneMinusSrcAlpha,

  kDstColor,
  kOneMinusDstColor,

  kDstAlpha,
  kOneMinusDstAlpha,

  kBlendColor,
  kOneMinusBlendColor,

  kBlendAlpha,
  kOneMinusBlendAlpha,

  kSrcAlphaSaturated,
};

enum class BlendOperation: uint8 {
  kAdd,
  kSubtract,
  kReverseSubtract,
  kMin,
  kMax,
};

enum class TextureType: uint8 {
  kTexture2d,
  kTexture3d,
  kTextureCube,
};

enum class BufferUsage: uint8 {
  kImmutable,
  kDynamic,
  kStream,
};

enum class CompareFunction: uint8 {
  kNever,
  kLess,
  kLessEqual,
  kGreater,
  kGreaterEqual,
  kEqual,
  kNotEqual,
  kAlways,
};

enum class StencilOperation: uint8 {
  kKeep,
  kZero,
  kReplace,
  kIncrement,
  kIncrementWrap,
  kDecrement,
  kDecrementWrap,
  kInvert,
};

enum class FillMode : uint8 {
  kWireframe,
  kSolid,
};

enum class FrontFaceType: uint8 {
  kClockWise,
  kCounterClockWise,
};

enum class FaceSide: uint8 {
  kFront,
  kBack,
  kBoth,
};

enum class IndexFormat: uint8 {
  kNone,
  kUint16,
  kUint32,
};

static size_t SizeOfIndexFormat(IndexFormat type) {
  auto size = 0;
  switch (type) {
    case IndexFormat::kNone:
      size = 0;
      break;
    case IndexFormat::kUint16:
      size = 2;
      break;
    case IndexFormat::kUint32:
      size = 4;
      break;
    default:
      break;
  }
  return size;
}

enum class VertexElementSemantic: uint8 {
  kPosition,
  kTexcoord0,
  kTexcoord1,
  kTexcoord2,
  kTexcoord3,
  kColor0,
  kColor1,
  kNormal,
  kTangent,
  kBinormal,
  kWeights,
  kIndices,
  kInstance0,
  kInstance1,
  kInstance2,
  kInstance3,

  kMaxSemanticCount,
  kInvalid
};

enum class VertexElementFormat: uint8 {
  kInvalid,
  kFloat1,
  kFloat2,
  kFloat3,
  kFloat4,
  kByte4,
  kByte4Normalized,
  kUnsignedByte4,
  kUnsignedByte4Normalized,
  kShort2,
  kShort2Normalized,
  kShort4,
  kShort4Normalized,
};

static bool IsNormalizedForVertexElementFormat(VertexElementFormat format)
{
  switch (format)
  {
    case VertexElementFormat::kFloat1: return false;
    case VertexElementFormat::kFloat2: return false;
    case VertexElementFormat::kFloat3: return false;
    case VertexElementFormat::kFloat4: return false;
    case VertexElementFormat::kByte4: return false;
    case VertexElementFormat::kByte4Normalized: return true;
    case VertexElementFormat::kUnsignedByte4: return false;
    case VertexElementFormat::kUnsignedByte4Normalized: return true;
    case VertexElementFormat::kShort2: return false;
    case VertexElementFormat::kShort2Normalized: return true;
    case VertexElementFormat::kShort4: return false;
    case VertexElementFormat::kShort4Normalized: return true;
    default: x_error("unknown VertexElementFormat\n"); return 0;
  }
}

static size_t SizeOfVertexElementFormat(VertexElementFormat format) {
  switch (format) {
    case VertexElementFormat::kFloat1:
      return 4;
    case VertexElementFormat::kFloat2:
      return 8;
    case VertexElementFormat::kFloat3:
      return 12;
    case VertexElementFormat::kFloat4:
      return 16;
    case VertexElementFormat::kByte4:
      return 4;
    case VertexElementFormat::kByte4Normalized:
      return 4;
    case VertexElementFormat::kUnsignedByte4:
      return 4;
    case VertexElementFormat::kUnsignedByte4Normalized:
      return 4;
    case VertexElementFormat::kShort2:
      return 4;
    case VertexElementFormat::kShort2Normalized:
      return 4;
    case VertexElementFormat::kShort4:
      return 8;
    case VertexElementFormat::kShort4Normalized:
      return 8;
    default:
      x_error("unknown VertexElementFormat\n");
      return 0;
  }
}

enum class VertexTopology: uint8 {
  kPoints,
  kLines,
  kLineStrip,
  kLineLoop,
  kTriangles,
  kTriangleStrip,
  kTriangleFan,
};

enum class TextureWrapMode: uint8 {
  kClampToEdge,
  kRepeat,
  kMirroredRepeat,
};

enum class TextureFilterMode: uint8 {
  kNearest,
  kLinear,
  kNearestMipmapNearest,
  kNearestMipmapLinear,
  kLinearMipmapNearest,
  kLinearMipmapLinear,
};

enum class ClearType: uint8 {
  kNone = 0,
  kColor = 1 << 0,
  kDepth = 1 << 1,
  kStencil = 1 << 2,

  kDepthAndStencil = static_cast<uint8>(kDepth) | static_cast<uint8>(kStencil),
  kAll = static_cast<uint8>(kColor) | static_cast<uint8>(kDepthAndStencil),
};

enum class GraphicsPipelineStage: uint8 {
  kVertexShader,
  kFragmentShader,
};

}

#endif // XENGINE_GRAPHICS_GRAPHICSDEFINE_H