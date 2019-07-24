#ifndef XENGINE_GRAPHICS_STATE_DEPTHSTENCILSTATE_H
#define XENGINE_GRAPHICS_STATE_DEPTHSTENCILSTATE_H

#include "graphics/GraphicsDefine.h"

namespace xEngine {

struct StencilState {
  union {
#pragma pack(push, 1)
    struct {
      StencilOperation fail : 4;
      StencilOperation depth_fail : 4;
      StencilOperation pass : 4;
      CompareFunction compare : 4;
    };
#pragma pack(pop)
    uint16 value;
  };

  StencilState();

  bool operator==(const StencilState &other) const;

  bool operator!=(const StencilState &other) const;
};

inline StencilState::StencilState() {
  static_assert(sizeof(StencilState) == sizeof(uint16), "wrong size of StencilState");
  value = 0;
  fail = StencilOperation::kKeep;
  depth_fail = StencilOperation::kKeep;
  pass = StencilOperation::kKeep;
  compare = CompareFunction::kAlways;
}

inline bool StencilState::operator==(const StencilState &other) const {
  return this->value == other.value;
}

inline bool StencilState::operator!=(const StencilState &other) const {
  return this->value != other.value;
}

struct DepthStencilState {
  StencilState front;

  StencilState back;

  union {
#pragma pack(push, 1)
    struct {
      bool depth_enable : 1;
      bool depth_write_enable : 1;
      CompareFunction depth_compare : 4;

      bool stencil_enable : 1;
      uint8 stencil_read_mask : 8;
      uint8 stencil_write_mask : 8;
      uint8 stencil_value : 8;
    };
#pragma pack(pop)
    uint32 value;
  };

  DepthStencilState();

  bool operator==(const DepthStencilState &other) const;

  bool operator!=(const DepthStencilState &other) const;
};

inline DepthStencilState::DepthStencilState() {
  static_assert(sizeof(DepthStencilState) == (sizeof(uint32) + 2 * sizeof(StencilState)), "wrong size of DepthStencilState");
  value = 0;
  depth_enable = false;
  depth_write_enable = false;
  depth_compare = CompareFunction::kLess;
  stencil_enable = false;
  stencil_read_mask = 0xff;
  stencil_write_mask = 0xff;
  stencil_value = 0;
}

inline bool DepthStencilState::operator==(const DepthStencilState &other) const {
  return this->value == other.value && this->front == other.front && this->back == other.back;
}

inline bool DepthStencilState::operator!=(const DepthStencilState &other) const {
  return this->value != other.value || this->front != other.front || this->back != other.back;
}

} // namespace xEngine

#endif // XENGINE_GRAPHICS_STATE_DEPTHSTENCILSTATE_H