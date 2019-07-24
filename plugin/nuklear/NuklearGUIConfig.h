#ifndef XENGINE_PLUGIN_NUKLEAR_NUKLEARCONFIG_H
#define XENGINE_PLUGIN_NUKLEAR_NUKLEARCONFIG_H

#include <core/Types.h>
#include <resource/Resource.h>

namespace xEngine {

struct NuklearGUIConfig {

  static NuklearGUIConfig ForWindow(ResourceID window);

  ResourceID window{kInvalidResourceID};

  uint32 max_vertex_count{64 * 1024};

  uint32 max_index_count{128 * 1024};

  float global_alpha{1.0f};

  bool line_AA{true};

  bool shape_AA{true};

  uint32 circle_segment_count{22};

  uint32 arc_segment_count{22};

  uint32 curve_segment_count{22};

};

inline NuklearGUIConfig NuklearGUIConfig::ForWindow(ResourceID window) {
  NuklearGUIConfig config;
  config.window = window;
  return config;
}

} // namespace xEngine

#endif // XENGINE_PLUGIN_NUKLEAR_NUKLEARCONFIG_H