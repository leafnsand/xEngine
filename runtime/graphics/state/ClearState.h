#ifndef XENGINE_GRAPHICS_STATE_CLEARSTATE_H
#define XENGINE_GRAPHICS_STATE_CLEARSTATE_H

#include "graphics/GraphicsDefine.h"

namespace xEngine {

struct ClearState {

  static ClearState ClearColor(Color clear_color = Color(1.0f, 1.0f, 1.0f, 1.0f)) {
    ClearState state;
    state.type = ClearType::kColor;
    state.clear_color = clear_color;
    return state;
  }

  static ClearState ClearDepth(float32 depth = 1.0f) {
    ClearState state;
    state.type = ClearType::kDepth;
    state.depth = depth;
    return state;
  }

  static ClearState ClearStencil(uint8 stencil = 0) {
    ClearState state;
    state.type = ClearType::kStencil;
    state.stencil = stencil;
    return state;
  }

  static ClearState ClearAll(Color clear_color = Color(0.0f, 0.0f, 0.0f, 1.0f), float32 depth = 1.0f, uint8 stencil = 0) {
    ClearState state;
    state.type = ClearType::kAll;
    state.clear_color = clear_color;
    state.depth = depth;
    state.stencil = stencil;
    return state;
  }

  Color clear_color = Color(0.0f, 0.0f, 0.0f, 0.0f);

  float32 depth = 1.0f;

  uint8 stencil = 0;

  ClearType type = ClearType::kNone;

};

}

#endif // XENGINE_GRAPHICS_STATE_CLEARSTATE_H