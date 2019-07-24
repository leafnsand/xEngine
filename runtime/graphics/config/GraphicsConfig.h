#ifndef XENGINE_GRAPHICS_CONFIG_GRAPHICSCONFIG_H
#define XENGINE_GRAPHICS_CONFIG_GRAPHICSCONFIG_H

#include "graphics/GraphicsDefine.h"

#include "resource/Resource.h"

namespace xEngine {

struct GraphicsConfig {

  static GraphicsConfig ForWindow(ResourceID id);

  ResourceID window{kInvalidResourceID};

#if X_D3D11
  GraphicsType type{GraphicsType::kD3D11};
#elif X_OPENGL
  GraphicsType type{GraphicsType::kOpenGL3};
#endif

  uint16 shader_pool_size{256};

  uint16 texture_pool_size{1024};

  uint16 mesh_pool_size{1024};

  uint16 pipeline_pool_size{256};

  uint16 sampler_pool_size{128};

  uint16 uniform_buffer_size{128};

  PixelFormat color_format{PixelFormat::RGBA8};

  PixelFormat depth_format{PixelFormat::D24S8};

  uint8 sample_count{8};

  int32 swap_interval{1};

};

inline GraphicsConfig GraphicsConfig::ForWindow(ResourceID id) {
  GraphicsConfig config;
  config.window = id;
  return config;
}

} // namespace xEngine

#endif // XENGINE_GRAPHICS_CONFIG_GRAPHICSCONFIG_H