#ifndef XENGINE_GRAPHICS_STATE_RASTERIZERSTATE_H
#define XENGINE_GRAPHICS_STATE_RASTERIZERSTATE_H

#include "graphics/GraphicsDefine.h"

namespace xEngine {

struct RasterizerState {
  union {
#pragma pack(push, 1)
    struct {
      bool cull_face_enable : 1;
      bool depth_offset_enable : 1;
      bool scissor_test_enable : 1;
      bool dither_enable : 1;
      bool alpha_to_coverage_enable : 1;
      FillMode fill_mode : 2;
      FrontFaceType front_face : 2;
      FaceSide cull_face : 2;
      uint8 sample : 4;
    };
#pragma pack(pop)
    uint16 value;
  };

  RasterizerState();

  bool operator==(const RasterizerState &other) const;

  bool operator!=(const RasterizerState &other) const;
};

inline RasterizerState::RasterizerState() {
  static_assert(sizeof(RasterizerState) == sizeof(uint16), "wrong size of RasterizerState");
  value = 0;
  cull_face_enable = false;
  depth_offset_enable = false;
  scissor_test_enable = false;
  dither_enable = true;
  alpha_to_coverage_enable = false;
  fill_mode = FillMode::kSolid;
  front_face = FrontFaceType::kClockWise;
  cull_face = FaceSide::kBack;
  sample = 1;
}

inline bool RasterizerState::operator==(const RasterizerState &other) const {
  return this->value == other.value;
}

inline bool RasterizerState::operator!=(const RasterizerState &other) const {
  return this->value != other.value;
}

} // namespace xEngine

#endif // XENGINE_GRAPHICS_STATE_RASTERIZERSTATE_H