#ifndef XENGINE_GRAPHICS_CONFIG_PIPELINECONFIG_H
#define XENGINE_GRAPHICS_CONFIG_PIPELINECONFIG_H

#include "MeshConfig.h"
#include "graphics/GraphicsDefine.h"
#include "graphics/GraphicsResourceSignature.h"

#include "graphics/state/BlendState.h"
#include "graphics/state/DepthStencilState.h"
#include "graphics/state/RasterizerState.h"

namespace xEngine {

struct PipelineConfig {

  static PipelineConfig ShaderWithLayout(ResourceID shader, VertexLayout layout) {
    PipelineConfig config;
    config.shader = shader;
    config.vertex_layout = layout;
    return config;
  }

  ResourceIdentity identity{ResourceIdentity::Shared(PipelineSignature)};

  ResourceID shader;

  VertexLayout vertex_layout;

  BlendState blend_state;

  DepthStencilState depth_stencil_state;

  RasterizerState rasterizer_state;

};

} // namespace xEngine

#endif // XENGINE_GRAPHICS_CONFIG_PIPELINECONFIG_H