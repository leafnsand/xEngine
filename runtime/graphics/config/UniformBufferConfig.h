#ifndef XENGINE_GRAPHICS_CONFIG_UNIFORMBUFFERCONFIG_H
#define XENGINE_GRAPHICS_CONFIG_UNIFORMBUFFERCONFIG_H

#include "graphics/GraphicsDefine.h"
#include "graphics/GraphicsResourceSignature.h"

#include <EASTL/string.h>
#include <EASTL/vector.h>

namespace xEngine {

struct UniformBufferConfig {

  ResourceIdentity identity{ResourceIdentity::Shared(UniformBufferSignature)};

  size_t size{0};

  BufferUsage usage{BufferUsage::kStream};

};

} // namespace xEngine

#endif // XENGINE_GRAPHICS_CONFIG_UNIFORMBUFFERCONFIG_H