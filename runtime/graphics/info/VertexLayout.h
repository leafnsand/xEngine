#ifndef XENGINE_GRAPHICS_INFO_VERTEXLAYOUT_H
#define XENGINE_GRAPHICS_INFO_VERTEXLAYOUT_H

#include "graphics/GraphicsDefine.h"

#include "core/Core.h"

namespace xEngine {

struct VertexLayout {

  VertexLayout() {
    memset(elements, 0, sizeof(elements[0]) * static_cast<uint16>(GraphicsMaxDefine::kMaxVertexElementCount));
  }

  VertexLayout &AddElement(VertexElementSemantic semantic, VertexElementFormat format) {
    if (semantic == VertexElementSemantic::kInvalid || format == VertexElementFormat::kInvalid) return *this;
    x_assert(element_count <= static_cast<uint16>(GraphicsMaxDefine::kMaxVertexElementCount));
    elements[element_count].semantic = semantic;
    elements[element_count].format = format;
    elements[element_count].offset = size;
    size += SizeOfVertexElementFormat(format);
    ++element_count;
    return *this;
  }

  struct {

    VertexElementSemantic semantic{ VertexElementSemantic::kInvalid };

    VertexElementFormat format{ VertexElementFormat::kInvalid };

    size_t offset{ 0 };

  } elements[static_cast<uint16>(GraphicsMaxDefine::kMaxVertexElementCount)];

  uint16 element_count{ 0 };

  size_t size{ 0 };

};

} // namespace xEngine

#endif // XENGINE_GRAPHICS_INFO_VERTEXLAYOUT_H
