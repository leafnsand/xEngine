#ifndef XENGINE_GRAPHICS_STATE_DRAWCALLSTATE_H
#define XENGINE_GRAPHICS_STATE_DRAWCALLSTATE_H

#include "graphics/GraphicsDefine.h"

namespace xEngine {

struct DrawCallState {

  static DrawCallState Triangles(size_t count) {
    DrawCallState state;
    state.count = count;
    return state;
  }

  size_t first{0};

  size_t count{0};

  VertexTopology topology{VertexTopology::kTriangles};

};

}

#endif // XENGINE_GRAPHICS_STATE_DRAWCALLSTATE_H