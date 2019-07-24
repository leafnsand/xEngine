#ifndef XENGINE_GRAPHICS_STATE_BLENDSTATE_H
#define XENGINE_GRAPHICS_STATE_BLENDSTATE_H

#include "graphics/GraphicsDefine.h"

#include "window/WindowDefine.h"

namespace xEngine {

struct BlendState {
  union {
#pragma pack(push, 1)
    struct {
      bool enable : 1;

      BlendFactor src_rgb_factor : 4;
      BlendFactor dst_rgb_factor : 4;
      BlendOperation rgb_operation : 3;

      BlendFactor src_alpha_factor : 4;
      BlendFactor dst_alpha_factor : 4;
      BlendOperation alpha_operation : 3;

      PixelChannel color_mask : 7;
      PixelFormat color_format : 7;
      PixelFormat depth_format : 7;
    };
#pragma pack(pop)
    uint64 value;
  };

  Color blend_color;

  BlendState();

  bool operator==(const BlendState &other) const;

  bool operator!=(const BlendState &other) const;
};

inline BlendState::BlendState() {
  static_assert(sizeof(BlendState) == sizeof(uint64) + sizeof(Color), "wrong size of BlendState");
  value = 0;
  enable = false;
  src_rgb_factor = BlendFactor::kOne;
  dst_rgb_factor = BlendFactor::kZero;
  rgb_operation = BlendOperation::kAdd;
  src_alpha_factor = BlendFactor::kOne;
  dst_alpha_factor = BlendFactor::kZero;
  alpha_operation = BlendOperation::kAdd;
  color_mask = PixelChannel::kRGBA;
  color_format = PixelFormat::RGBA8;
  depth_format = PixelFormat::D24S8;
  blend_color = Color();
}

inline bool BlendState::operator==(const BlendState &other) const {
  return this->value == other.value && this->blend_color == other.blend_color;
}

inline bool BlendState::operator!=(const BlendState &other) const {
  return this->value != other.value || this->blend_color != other.blend_color;
}

} // namespace xEngine

#endif // XENGINE_GRAPHICS_STATE_BLENDSTATE_H