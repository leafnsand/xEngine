#ifndef XENGINE_GRAPHICS_CONFIG_SHADERCONFIG_H
#define XENGINE_GRAPHICS_CONFIG_SHADERCONFIG_H

#include "core/Data.h"

#include "graphics/GraphicsDefine.h"
#include "graphics/GraphicsResourceSignature.h"

#include <EASTL/string.h>

namespace xEngine {

struct ShaderConfig {

  static ShaderConfig FromSource(const eastl::string &source);

  static ShaderConfig FromData(const DataPtr &vertex, const DataPtr &fragment);

  ResourceIdentity identity{ResourceIdentity::Shared(ShaderSignature)};

  eastl::string source;

  DataPtr vertex;

  DataPtr fragment;

};

inline ShaderConfig ShaderConfig::FromSource(const eastl::string &source) {
  ShaderConfig config;
  config.source = source;
  return config;
}

inline ShaderConfig ShaderConfig::FromData(const DataPtr &vertex, const DataPtr &fragment) {
  ShaderConfig config;
  config.vertex = Data::Create(vertex->buffer(), vertex->size());
  config.fragment = Data::Create(fragment->buffer(), fragment->size());
  return config;
}

} // namespace xEngine

#endif // XENGINE_GRAPHICS_CONFIG_SHADERCONFIG_H