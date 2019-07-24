#ifndef XENGINE_GRAPHICS_IMPL_D3D11_D3D11DEFINE_H
#define XENGINE_GRAPHICS_IMPL_D3D11_D3D11DEFINE_H

#if X_D3D11

#include "graphics/GraphicsDefine.h"
#include "graphics/config/GraphicsConfig.h"
#include "graphics/state/BlendState.h"
#include "graphics/state/DepthStencilState.h"
#include "graphics/state/RasterizerState.h"

#include "graphics/impl/D3D11/resource/D3D11GraphicsResource.h"

#include "window/WindowDefine.h"

#include <D3D11.h>

#define x_d3d11_assert(call) x_assert(SUCCEEDED(call))

#define x_d3d11_assert_msg(call, msg) x_assert_msg(SUCCEEDED(call), msg)

namespace xEngine {

struct D3DRendererCache {
  ID3D11RenderTargetView *render_target_view{nullptr};

  ID3D11DepthStencilView *depth_stencil_view{nullptr};

  ID3D11BlendState *blend_state{nullptr};

  ID3D11DepthStencilState *depth_stencil_state{nullptr};

  ID3D11RasterizerState *rasterizer_state{nullptr};

  D3D11_VIEWPORT viewport;

  D3D11_RECT scissor;

  Color blend_color;

  ID3D11InputLayout *input_layout{nullptr};

  ID3D11Buffer *vertex_buffer{nullptr};

  IndexFormat index_format{IndexFormat::kNone};
  ID3D11Buffer *index_buffer{nullptr};

  ID3D11VertexShader *vertex_shader{nullptr};

  ID3D11PixelShader *fragment_shader{nullptr};

  ID3D11ShaderResourceView *vertex_shader_resource_view[static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount)];

  ID3D11SamplerState *vertex_sampler_state[static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount)];

  ID3D11ShaderResourceView *fragment_shader_resource_view[static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount)];

  ID3D11SamplerState *fragment_sampler_state[static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount)];

  D3DRendererCache() {
    memset(vertex_shader_resource_view, 0, sizeof(vertex_shader_resource_view));
    memset(vertex_sampler_state, 0, sizeof(vertex_sampler_state));
    memset(fragment_shader_resource_view, 0, sizeof(fragment_shader_resource_view));
    memset(fragment_sampler_state, 0, sizeof(fragment_sampler_state));
  }
};

static DXGI_FORMAT SwapChainFormatForPixelFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    default:
      x_error("unknown PixelFormat for swap chain\n");
      return DXGI_FORMAT_UNKNOWN;
  }
}

static DXGI_FORMAT RenderTargetFormatForPixelFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA8:
      return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat::RGBA32F:
      return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case PixelFormat::RGBA16F:
      return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case PixelFormat::D16:
      return DXGI_FORMAT_D16_UNORM;
    case PixelFormat::D24S8:
      return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default:
      x_error("unknown PixelFormat for render target\n");
      return DXGI_FORMAT_UNKNOWN;
  }
}

static D3D11_USAGE D3D11UsageForBufferUsage(BufferUsage usage) {
  switch (usage) {
    case BufferUsage::kImmutable:
      return D3D11_USAGE_IMMUTABLE;
    case BufferUsage::kDynamic:
      return D3D11_USAGE_DYNAMIC;
    case BufferUsage::kStream:
      return D3D11_USAGE_DYNAMIC;
    default:
      x_error("unknown BufferUsage\n");
      return D3D11_USAGE_IMMUTABLE;
  }
}

static UINT D3D11CPUAccessFlagForBufferUsage(BufferUsage usage) {
  switch (usage) {
    case BufferUsage::kImmutable:
      return 0;
    case BufferUsage::kDynamic:
      return D3D11_CPU_ACCESS_WRITE;
    case BufferUsage::kStream:
      return D3D11_CPU_ACCESS_WRITE;
    default:
      x_error("unknown BufferUsage\n");
      return 0;
  }
}

static DXGI_FORMAT TextureFormatForPixelFormat(PixelFormat format) {
  switch (format) {
    case PixelFormat::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case PixelFormat::RGBA4: return DXGI_FORMAT_B4G4R4A4_UNORM;
    case PixelFormat::R5G6B5: return DXGI_FORMAT_B5G6R5_UNORM;
    case PixelFormat::R5G5B5A1: return DXGI_FORMAT_B5G5R5A1_UNORM;
    case PixelFormat::RGBA32F: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case PixelFormat::RGBA16F: return DXGI_FORMAT_R16G16B16A16_FLOAT;
    case PixelFormat::L8: return DXGI_FORMAT_R8_UNORM;
    case PixelFormat::DXT1: return DXGI_FORMAT_BC1_UNORM;
    case PixelFormat::DXT3: return DXGI_FORMAT_BC2_UNORM;
    case PixelFormat::DXT5: return DXGI_FORMAT_BC3_UNORM;
    case PixelFormat::D16: return DXGI_FORMAT_D16_UNORM;
    case PixelFormat::D32: return DXGI_FORMAT_D32_FLOAT;
    case PixelFormat::D24S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
    default: x_error("unknown PixelFormat\n"); return DXGI_FORMAT_UNKNOWN;
  }
}

static D3D11_FILTER EnumForTextureFilterMode(TextureFilterMode min, TextureFilterMode mag) {
  if (TextureFilterMode::kNearest == mag) {
    switch (min) {
      case TextureFilterMode::kNearest:
      case TextureFilterMode::kNearestMipmapNearest:
        return D3D11_FILTER_MIN_MAG_MIP_POINT;
      case TextureFilterMode::kLinear:
      case TextureFilterMode::kLinearMipmapNearest:
        return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
      case TextureFilterMode::kNearestMipmapLinear:
        return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
      case TextureFilterMode::kLinearMipmapLinear:
        return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
      default:
        break;
    }
  } else if (TextureFilterMode::kLinear == mag) {
    switch (mag) {
      case TextureFilterMode::kNearest:
      case TextureFilterMode::kNearestMipmapNearest:
        return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
      case TextureFilterMode::kLinear:
      case TextureFilterMode::kLinearMipmapNearest:
        return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
      case TextureFilterMode::kNearestMipmapLinear:
        return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
      case TextureFilterMode::kLinearMipmapLinear:
        return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
      default:
        break;
    }
  }
  x_error("SamplerFilterForTextureFilterMode invalid filter combination!\n");
  return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

static D3D11_TEXTURE_ADDRESS_MODE EnumForTextureWrapMode(TextureWrapMode mode) {
  switch (mode) {
    case TextureWrapMode::kClampToEdge:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
    case TextureWrapMode::kRepeat:
      return D3D11_TEXTURE_ADDRESS_WRAP;
    case TextureWrapMode::kMirroredRepeat:
      return D3D11_TEXTURE_ADDRESS_MIRROR;
    default:
      return D3D11_TEXTURE_ADDRESS_CLAMP;
  }
}

static uint32 RowPitchForPixelFormat(PixelFormat format, int32 width) {
  switch (format) {
    case PixelFormat::DXT1:
    case PixelFormat::ETC2_RGB8:
    case PixelFormat::ETC2_SRGB8:
      return eastl::max(8, 2 * width + 6);
    case PixelFormat::DXT3:
    case PixelFormat::DXT5:
      return eastl::max(16, 4 * width + 12);
    case PixelFormat::PVRTC4_RGB:
    case PixelFormat::PVRTC4_RGBA:
    case PixelFormat::PVRTC2_RGB:
    case PixelFormat::PVRTC2_RGBA:
      return eastl::max(16, 2 * width);
    default:
      return width * SizeOfPixelFormat(format);
  }
}

static D3D11_BLEND EnumForBlendFactor(BlendFactor factor) {
  switch (factor) {
    case BlendFactor::kZero: return D3D11_BLEND_ZERO;
    case BlendFactor::kOne: return D3D11_BLEND_ONE;
    case BlendFactor::kSrcColor: return D3D11_BLEND_SRC_COLOR;
    case BlendFactor::kOneMinusBlendColor: return D3D11_BLEND_INV_SRC_COLOR;
    case BlendFactor::kSrcAlpha: return D3D11_BLEND_SRC_ALPHA;
    case BlendFactor::kOneMinusSrcAlpha: return D3D11_BLEND_INV_SRC_ALPHA;
    case BlendFactor::kDstColor: return D3D11_BLEND_DEST_COLOR;
    case BlendFactor::kOneMinusDstColor: return D3D11_BLEND_INV_DEST_COLOR;
    case BlendFactor::kDstAlpha: return D3D11_BLEND_DEST_ALPHA;
    case BlendFactor::kOneMinusDstAlpha: return D3D11_BLEND_INV_DEST_ALPHA;
    case BlendFactor::kSrcAlphaSaturated: return D3D11_BLEND_SRC_ALPHA_SAT;
    case BlendFactor::kBlendColor: return D3D11_BLEND_BLEND_FACTOR;
    case BlendFactor::kOneMinusBlendAlpha: return D3D11_BLEND_INV_BLEND_FACTOR;
    default: x_error("unsupport blend factor\n"); return D3D11_BLEND_ONE;
  }
}

static D3D11_BLEND_OP EnumForBlendOperation(BlendOperation operation) {
  switch (operation) {
    case BlendOperation::kAdd: return D3D11_BLEND_OP_ADD;
    case BlendOperation::kSubtract: return D3D11_BLEND_OP_SUBTRACT;
    case BlendOperation::kReverseSubtract: return D3D11_BLEND_OP_REV_SUBTRACT;
    case BlendOperation::kMin: return D3D11_BLEND_OP_MIN;
    case BlendOperation::kMax: return D3D11_BLEND_OP_MAX;
    default: x_error("unsupport blend operation\n"); return D3D11_BLEND_OP_ADD;
  }
}

static uint8 ColorWriteMaskForPixelChannel(PixelChannel mask) {
  uint8 result = 0;
  if (static_cast<uint8>(mask) & static_cast<uint8>(PixelChannel::kRed)) {
    result |= D3D11_COLOR_WRITE_ENABLE_RED;
  }
  if (static_cast<uint8>(mask) & static_cast<uint8>(PixelChannel::kGreen)) {
    result |= D3D11_COLOR_WRITE_ENABLE_GREEN;
  }
  if (static_cast<uint8>(mask) & static_cast<uint8>(PixelChannel::kBlue)) {
    result |= D3D11_COLOR_WRITE_ENABLE_BLUE;
  }
  if (static_cast<uint8>(mask) & static_cast<uint8>(PixelChannel::kAlpha)) {
    result |= D3D11_COLOR_WRITE_ENABLE_ALPHA;
  }
  return result;
}

static D3D11_COMPARISON_FUNC EnumForCompareFunction(CompareFunction function) {
  switch (function) {
    case CompareFunction::kNever: return D3D11_COMPARISON_NEVER;
    case CompareFunction::kLess: return D3D11_COMPARISON_LESS;
    case CompareFunction::kEqual: return D3D11_COMPARISON_EQUAL;
    case CompareFunction::kLessEqual: return D3D11_COMPARISON_LESS_EQUAL;
    case CompareFunction::kGreater: return D3D11_COMPARISON_GREATER;
    case CompareFunction::kNotEqual: return D3D11_COMPARISON_NOT_EQUAL;
    case CompareFunction::kGreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
    case CompareFunction::kAlways: return D3D11_COMPARISON_ALWAYS;
    default: x_error("unsupport CompareFunction\n"); return D3D11_COMPARISON_ALWAYS;
  }
}

static D3D11_STENCIL_OP EnumForStencilOperation(StencilOperation operation) {
  switch (operation) {
    case StencilOperation::kKeep: return D3D11_STENCIL_OP_KEEP;
    case StencilOperation::kZero: return D3D11_STENCIL_OP_ZERO;
    case StencilOperation::kReplace: return D3D11_STENCIL_OP_REPLACE;
    case StencilOperation::kIncrement: return D3D11_STENCIL_OP_INCR;
    case StencilOperation::kIncrementWrap: return D3D11_STENCIL_OP_INCR_SAT;
    case StencilOperation::kDecrement: return D3D11_STENCIL_OP_DECR;
    case StencilOperation::kDecrementWrap: return D3D11_STENCIL_OP_DECR_SAT;
    case StencilOperation::kInvert: return D3D11_STENCIL_OP_INVERT;
    default: x_error("unsupport StencilOperation\n"); return D3D11_STENCIL_OP_ZERO;
  }
}

static const char *SemanticForVertexElementSemantic(VertexElementSemantic semantic) {
  switch (semantic) {
    case VertexElementSemantic::kPosition:
      return "POSITION";
    case VertexElementSemantic::kTexcoord0:
    case VertexElementSemantic::kTexcoord1:
    case VertexElementSemantic::kTexcoord2:
    case VertexElementSemantic::kTexcoord3:
      return "TEXCOORD";
    case VertexElementSemantic::kColor0:
    case VertexElementSemantic::kColor1:
      return "COLOR";
    case VertexElementSemantic::kNormal:
      return "NORMAL";
    case VertexElementSemantic::kTangent:
      return "TANGENT";
    case VertexElementSemantic::kBinormal:
      return "BINORMAL";
    case VertexElementSemantic::kWeights:
      return "BLENDWEIGHT";
    case VertexElementSemantic::kIndices:
      return "BLENDINDICES";
    case VertexElementSemantic::kInstance0:
    case VertexElementSemantic::kInstance1:
    case VertexElementSemantic::kInstance2:
    case VertexElementSemantic::kInstance3:
      return "INSTANCE";
    default: 
      x_error("unsupport VertexElementSemantic\n"); 
      return nullptr;
  }
}

static uint32 IndexForVertexElementSemantic(VertexElementSemantic semantic) {
  switch (semantic) {
    case VertexElementSemantic::kTexcoord1:
    case VertexElementSemantic::kColor1:
    case VertexElementSemantic::kInstance1:
      return 1;
    case VertexElementSemantic::kTexcoord2:
    case VertexElementSemantic::kInstance2:
      return 2;
    case VertexElementSemantic::kTexcoord3:
    case VertexElementSemantic::kInstance3:
      return 3;
    default:
      return 0;
  }
}

static VertexElementSemantic VertexElementSemanticForSemanticAndIndex(const char *semantic, uint32 index) {
  if (strcmp(semantic, "POSITION") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kPosition;
      default: 
        x_error("unsupport semantic POSITION index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "TEXCOORD") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kTexcoord0;
      case 1: return VertexElementSemantic::kTexcoord1;
      case 2: return VertexElementSemantic::kTexcoord2;
      case 3: return VertexElementSemantic::kTexcoord3;
      default: 
        x_error("unsupport semantic TEXCOORD index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "COLOR") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kColor0;
      case 1: return VertexElementSemantic::kColor1;
      default: 
        x_error("unsupport semantic COLOR index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "NORMAL") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kNormal;
      default: 
        x_error("unsupport semantic NORMAL index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "TANGENT") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kTangent;
      default: 
        x_error("unsupport semantic TANGENT index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "BINORMAL") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kBinormal;
      default: 
        x_error("unsupport semantic BINORMAL index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "BLENDWEIGHT") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kWeights;
      default: 
        x_error("unsupport semantic BLENDWEIGHT index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "BLENDINDICES") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kIndices;
      default: 
        x_error("unsupport semantic BLENDINDICES index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else if (strcmp(semantic, "INSTANCE") == 0) {
    switch (index) {
      case 0: return VertexElementSemantic::kInstance0;
      case 1: return VertexElementSemantic::kInstance1;
      case 2: return VertexElementSemantic::kInstance2;
      case 3: return VertexElementSemantic::kInstance3;
      default: 
        x_error("unsupport semantic INSTANCE index\n");
        return VertexElementSemantic::kInvalid;
    }
  } else {
    return VertexElementSemantic::kInvalid;
  }
}

static DXGI_FORMAT EnumForVertexElementFormat(VertexElementFormat format) {
  switch (format) {
    case VertexElementFormat::kFloat1: return DXGI_FORMAT_R32_FLOAT;
    case VertexElementFormat::kFloat2: return DXGI_FORMAT_R32G32_FLOAT;
    case VertexElementFormat::kFloat3: return DXGI_FORMAT_R32G32B32_FLOAT;
    case VertexElementFormat::kFloat4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
    case VertexElementFormat::kByte4: return DXGI_FORMAT_R8G8B8A8_SINT;
    case VertexElementFormat::kByte4Normalized: return DXGI_FORMAT_R8G8B8A8_SNORM;
    case VertexElementFormat::kUnsignedByte4: return DXGI_FORMAT_R8G8B8A8_UINT;
    case VertexElementFormat::kUnsignedByte4Normalized: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case VertexElementFormat::kShort2: return DXGI_FORMAT_R16G16_SINT;
    case VertexElementFormat::kShort2Normalized: return DXGI_FORMAT_R16G16_SNORM;
    case VertexElementFormat::kShort4: return DXGI_FORMAT_R16G16B16A16_SINT;
    case VertexElementFormat::kShort4Normalized: return DXGI_FORMAT_R16G16B16A16_SNORM;
    default: x_error("unsupport VertexElementFormat!\n"); return DXGI_FORMAT_UNKNOWN;
  }
}

static VertexElementFormat VertexElementFormatForComponentTypeAndMask(D3D_REGISTER_COMPONENT_TYPE type, uint8_t mask) {
  if (type == D3D_REGISTER_COMPONENT_FLOAT32) {
    if (mask == 1) {
      return VertexElementFormat::kFloat1;
    } else if (mask == 3) {
      return VertexElementFormat::kFloat2;
    } else if (mask == 7) {
      return VertexElementFormat::kFloat3;
    } else if (mask == 15) {
      return VertexElementFormat::kFloat4;
    }
  } else if (type == D3D_REGISTER_COMPONENT_SINT32) {
    // TODO
  } else if (type == D3D_REGISTER_COMPONENT_UINT32) {
    // TODO
  }
  x_error("Invalid component type and mask\n");
  return VertexElementFormat::kInvalid;
}

static DXGI_FORMAT EnumForIndexFormat(IndexFormat format) {
  switch (format) {
    case IndexFormat::kUint16: return DXGI_FORMAT_R16_UINT;
    case IndexFormat::kUint32: return DXGI_FORMAT_R32_UINT;
    default: x_error("unsupport IndexFormat!\n"); return DXGI_FORMAT_UNKNOWN;
  }
}

static D3D11_PRIMITIVE_TOPOLOGY EnumForVertexTopology(VertexTopology topology) {
  switch (topology) {
    case VertexTopology::kPoints: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case VertexTopology::kLines: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case VertexTopology::kLineStrip: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case VertexTopology::kTriangles: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case VertexTopology::kTriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    default: x_error("unsupport VertexTopology!\n"); return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
  }
}

} // namespace xEngine

#endif // X_D3D11

#endif // XENGINE_GRAPHICS_IMPL_D3D11_D3D11DEFINE_H
