#ifndef XENGINE_GRAPHICS_CONFIG_MESHCONFIG_H
#define XENGINE_GRAPHICS_CONFIG_MESHCONFIG_H

#include "graphics/GraphicsDefine.h"
#include "graphics/GraphicsResourceSignature.h"
#include "graphics/info/VertexLayout.h"

#include "core/Data.h"

namespace xEngine {

struct MeshConfig {

  ResourceIdentity identity{ResourceIdentity::Shared(MeshSignature)};

  BufferUsage index_usage{BufferUsage::kImmutable};

  IndexFormat index_type{IndexFormat::kNone};

  size_t index_count{0};

  DataPtr index_data;

  BufferUsage vertex_usage{BufferUsage::kImmutable};

  size_t vertex_count{0};

  DataPtr vertex_data;

  VertexLayout layout;

};

} // namespace xEngine

#endif // XENGINE_GRAPHICS_CONFIG_MESHCONFIG_H