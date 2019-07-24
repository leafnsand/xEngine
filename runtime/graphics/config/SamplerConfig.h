#ifndef XENGINE_GRAPHICS_CONFIG_SAMPLERCONFIG_H
#define XENGINE_GRAPHICS_CONFIG_SAMPLERCONFIG_H

#include "graphics/GraphicsDefine.h"
#include "graphics/GraphicsResourceSignature.h"

namespace xEngine {

struct SamplerConfig {

  ResourceIdentity identity{ResourceIdentity::Shared(SamplerSignature)};

  TextureFilterMode filter_mode_min{TextureFilterMode::kNearest};

  TextureFilterMode filter_mode_mag{TextureFilterMode::kNearest};

  TextureWrapMode wrap_mod_s{TextureWrapMode::kRepeat};

  TextureWrapMode wrap_mod_t{TextureWrapMode::kRepeat};

  TextureWrapMode wrap_mod_r{TextureWrapMode::kRepeat};

  float32 min_lod{-1000.0f};

  float32 max_lod{1000.0f};

  float32 lod_bias{0.0f};

  Color border_color;

  CompareFunction compare{CompareFunction::kNever};

};

} // namespace xEngine

#endif // XENGINE_GRAPHICS_CONFIG_SAMPLERCONFIG_H