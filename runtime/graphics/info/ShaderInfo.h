#ifndef XENGINE_GRAPHICS_INFO_SHADERINFO_H
#define XENGINE_GRAPHICS_INFO_SHADERINFO_H

#include "VertexLayout.h"

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

namespace xEngine {

struct ShaderInfo {

  struct UniformElementInfo {
    size_t offset{ 0 };
    size_t size{ 0 };
  };

  struct UniformBlockInfo {
    size_t location{ 0 };
    size_t size{ 0 };
    eastl::hash_map<eastl::string, UniformElementInfo> elements;
  };

  struct PipelineStageInfo {
    UniformBlockInfo global_uniform_block_info;
    eastl::hash_map<eastl::string, UniformBlockInfo> uniform_block_info;
    eastl::hash_map<eastl::string, uint32> texture_index;
    eastl::hash_map<eastl::string, uint32> sampler_index;
  };

  PipelineStageInfo vertex;
 
  PipelineStageInfo fragment;

  VertexLayout input_layout;

};

}

#endif // XENGINE_GRAPHICS_INFO_SHADERINFO_H