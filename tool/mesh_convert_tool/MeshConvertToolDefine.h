#ifndef XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTTOOLDEFINE_H
#define XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTTOOLDEFINE_H

#include <util/MeshUtil.h>
#include <application/ApplicationDelegate.h>

#include <EASTL/string.h>
#include <EASTL/vector.h>

#include <glm.hpp>

namespace xEngine {

struct MeshInfo {
  eastl::string name;
  MeshUtil util;
};

struct NodeInfo {
  eastl::string name;
  glm::mat4 transform;
  eastl::vector<int32> meshes;
  eastl::vector<NodeInfo> children;
};

struct SceneInfo {
  eastl::string file_name;
  NodeInfo root;
  eastl::vector<MeshInfo> meshes;
};

struct PostProcessFlag {
  int32 calc_tangent_space{0};
  int32 join_identical_vertices{0};
  int32 make_left_handed{0};
  int32 triangulate{1};
  int32 remove_component{0};
  int32 gen_normals{0};
  int32 gen_smooth_normals{0};
  int32 split_large_meshes{0};
  int32 pre_transform_vertices{0};
  int32 limit_bone_weights{0};
  int32 validate_data_structure{0};
  int32 improve_cache_locality{0};
  int32 remove_redundant_materials{0};
  int32 fix_infacing_normals{0};
  int32 sort_by_ptype{0};
  int32 find_degenerates{0};
  int32 find_invalid_data{0};
  int32 gen_uv_coords{0};
  int32 transform_uv_coords{0};
  int32 find_instances{0};
  int32 optimize_meshes{0};
  int32 optimize_graph{0};
  int32 flip_uvs{1};
  int32 flip_winding_order{1};
  int32 split_by_bone_count{0};
  int32 debone{0};
};

struct RemoveComponentFlag {
  int32 normals{0};
  int32 tangents_and_bitangents{0};
  int32 colors{0};
  int32 colors0{0};
  int32 colors1{0};
  int32 colors2{0};
  int32 colors3{0};
  int32 texcoords{0};
  int32 texcoords0{0};
  int32 texcoords1{0};
  int32 texcoords2{0};
  int32 texcoords3{0};
  int32 boneweights{0};
  int32 animations{0};
  int32 textures{0};
  int32 lights{0};
  int32 cameras{0};
  int32 meshes{0};
  int32 materials{0};
};

struct RemoveTopologyFlag {
  int32 point{1};
  int32 line{1};
  int32 triangle{0};
  int32 polygon{1};
};

} // namespace xEngine

#endif // XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTTOOLDEFINE_H
