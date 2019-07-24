#include "MeshConverter.h"
#include "MeshViewerWindow.h"

#include <application/Application.h>
#include <core/Log.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace xEngine {

PostProcessFlag MeshConverter::post_process_flag;

RemoveComponentFlag MeshConverter::remove_component_flag;

RemoveTopologyFlag MeshConverter::remove_topology_flag;

MeshViewerWindow *MeshConverter::viewer_ = nullptr;

void MeshConverter::Convert(const eastl::string &in, const eastl::string &out) {
  Assimp::Importer importer;
  importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, GetRemoveComponentFlag());
  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, GetRemoveTopologyFlag());

  auto scene = importer.ReadFile(in.c_str(), GetPostProcessFlag());
  if (scene == nullptr || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
    Log::GetInstance().Warning("Load %s Error:\n%s\n", in.c_str(), importer.GetErrorString());
    return;
  }

  SceneInfo scene_info;
  scene_info.file_name = out;

  for (auto mesh_index = 0; mesh_index < scene->mNumMeshes; ++mesh_index) {
    MeshInfo mesh_info;
    ProcessMesh(mesh_info, scene->mMeshes[mesh_index]);
    scene_info.meshes.push_back(mesh_info);
  }
  
  ProcessNode(scene_info.root, scene->mRootNode);

  importer.FreeScene();

  if (viewer_ == nullptr) viewer_ = new MeshViewerWindow;

  viewer_->Show(scene_info);

  SaveContext(scene_info);
}

void MeshConverter::ProcessMeshConfig(MeshUtil &util, aiMesh *mesh) {
  util.config().vertex_count = static_cast<size_t>(mesh->mNumVertices);

  for (auto face_index = 0; face_index < mesh->mNumFaces; ++face_index) {
    auto &face = mesh->mFaces[face_index];
    if (face.mNumIndices == 0) continue;
    if (face.mNumIndices == 1 && remove_topology_flag.point) continue;
    if (face.mNumIndices == 2 && remove_topology_flag.line) continue;
    if (face.mNumIndices == 3 && remove_topology_flag.triangle) continue;
    if (face.mNumIndices > 3 && remove_topology_flag.polygon) continue;
    util.config().index_count += face.mNumIndices;
  }

  util.config().index_type = util.config().index_count > eastl::numeric_limits<uint16>::max() ? IndexFormat::kUint32 : IndexFormat::kUint16;

  util.config().layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);

  if (!remove_component_flag.texcoords) {
    if (mesh->mNumUVComponents[0] && !remove_component_flag.texcoords0) {
      util.config().layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
    }
    if (mesh->mNumUVComponents[1] && !remove_component_flag.texcoords1) {
      util.config().layout.AddElement(VertexElementSemantic::kTexcoord1, VertexElementFormat::kFloat2);
    }
    if (mesh->mNumUVComponents[2] && !remove_component_flag.texcoords2) {
      util.config().layout.AddElement(VertexElementSemantic::kTexcoord2, VertexElementFormat::kFloat2);
    }
    if (mesh->mNumUVComponents[3] && !remove_component_flag.texcoords3) {
      util.config().layout.AddElement(VertexElementSemantic::kTexcoord3, VertexElementFormat::kFloat2);
    }
  }

  if (!remove_component_flag.colors) {
    if (mesh->mColors[0] && !remove_component_flag.colors0) {
      util.config().layout.AddElement(VertexElementSemantic::kColor0, VertexElementFormat::kFloat4);
    }
    if (mesh->mColors[1] && !remove_component_flag.colors1) {
      util.config().layout.AddElement(VertexElementSemantic::kColor1, VertexElementFormat::kFloat4);
    }
  }

  if (mesh->mNormals && !remove_component_flag.normals) {
    util.config().layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
  }

  if (mesh->mTangents && !remove_component_flag.tangents_and_bitangents) {
    util.config().layout.AddElement(VertexElementSemantic::kTangent, VertexElementFormat::kFloat3);
  }

  if (mesh->mBitangents && !remove_component_flag.tangents_and_bitangents) {
    util.config().layout.AddElement(VertexElementSemantic::kBinormal, VertexElementFormat::kFloat3);
  }
}

void MeshConverter::ProcessMeshVertexPosition(MeshUtil &util, aiMesh *mesh) {
  for (auto index = 0; index < mesh->mNumVertices; ++index) {
    auto &vertex = mesh->mVertices[index];
    util.Vertex(VertexElementSemantic::kPosition,
                static_cast<size_t>(index),
                static_cast<float32>(vertex.x),
                static_cast<float32>(vertex.y),
                static_cast<float32>(vertex.z));
  }
}

void MeshConverter::ProcessMeshVertexTexcoord(MeshUtil &util, aiMesh *mesh) {
  for (auto uv_layer_index = 0; uv_layer_index < mesh->GetNumUVChannels(); ++uv_layer_index) {
    if (uv_layer_index == 0 && remove_component_flag.texcoords0) continue;
    if (uv_layer_index == 1 && remove_component_flag.texcoords1) continue;
    if (uv_layer_index == 2 && remove_component_flag.texcoords2) continue;
    if (uv_layer_index == 3 && remove_component_flag.texcoords3) continue;
    if (uv_layer_index > 3) continue;
    auto uv_layer = mesh->mTextureCoords[uv_layer_index];
    auto semantic = static_cast<VertexElementSemantic>(static_cast<uint8>(VertexElementSemantic::kTexcoord0) + uv_layer_index);
    for (auto index = 0; index < mesh->mNumVertices; ++index) {
      util.Vertex(semantic,
                  static_cast<size_t>(index),
                  static_cast<float32>(uv_layer[index].x),
                  static_cast<float32>(uv_layer[index].y));
    }
  }
}

void MeshConverter::ProcessMeshVertexNormal(MeshUtil &util, aiMesh *mesh) {
  if (remove_component_flag.normals) return;
  for (auto index = 0; index < mesh->mNumVertices; ++index) {
    util.Vertex(VertexElementSemantic::kNormal,
                static_cast<size_t>(index),
                static_cast<float32>(mesh->mNormals[index].x),
                static_cast<float32>(mesh->mNormals[index].y),
                static_cast<float32>(mesh->mNormals[index].z));
  }
}

void MeshConverter::ProcessMeshVertex(MeshUtil &util, aiMesh *mesh) {
  util.BeginVertex();
  ProcessMeshVertexPosition(util, mesh);
  ProcessMeshVertexTexcoord(util, mesh);
  ProcessMeshVertexNormal(util, mesh);
  util.EndVertex();
}

void MeshConverter::ProcessMeshIndex(MeshUtil &util, aiMesh *mesh) {
  util.BeginIndex();
  for (auto face_index = 0; face_index < mesh->mNumFaces; ++face_index) {
    auto &face = mesh->mFaces[face_index];
    if (face.mNumIndices == 0) continue;
    if (face.mNumIndices == 1 && remove_topology_flag.point) continue;
    if (face.mNumIndices == 2 && remove_topology_flag.line) continue;
    if (face.mNumIndices == 3 && remove_topology_flag.triangle) continue;
    if (face.mNumIndices > 3 && remove_topology_flag.polygon) continue;
    for (auto position_index = 0; position_index < face.mNumIndices; ++position_index) {
      if (util.config().index_type == IndexFormat::kUint16) {
        util.Index16(static_cast<uint16>(face.mIndices[position_index]));
      } else {
        util.Index32(static_cast<uint32>(face.mIndices[position_index]));
      }
    }
  }
  util.EndIndex();
}

void MeshConverter::ProcessMesh(MeshInfo &info, aiMesh *mesh) {
  info.name = mesh->mName.C_Str();
  ProcessMeshConfig(info.util, mesh);
  ProcessMeshVertex(info.util, mesh);
  ProcessMeshIndex(info.util, mesh);
}

void MeshConverter::ProcessNode(NodeInfo &info, aiNode *node) {
  info.name = node->mName.C_Str();
  for (auto mesh_index = 0; mesh_index < node->mNumMeshes; ++mesh_index) {
    info.meshes.push_back(node->mMeshes[mesh_index]);
  }
  info.transform = glm::mat4(node->mTransformation.a1, node->mTransformation.b1, node->mTransformation.c1, node->mTransformation.d1,
                             node->mTransformation.a2, node->mTransformation.b2, node->mTransformation.c2, node->mTransformation.d2,
                             node->mTransformation.a3, node->mTransformation.b3, node->mTransformation.c3, node->mTransformation.d3,
                             node->mTransformation.a4, node->mTransformation.b4, node->mTransformation.c4, node->mTransformation.d4);
  
  for (auto node_index = 0; node_index < node->mNumChildren; ++node_index) {
    NodeInfo node_info;
    ProcessNode(node_info, node->mChildren[node_index]);
    info.children.push_back(node_info);
  }
}

void MeshConverter::SaveContext(SceneInfo &scene) {
  auto file = fopen(scene.file_name.c_str(), "wb");

  fclose(file);
}

uint32 MeshConverter::GetPostProcessFlag() {
  uint32 flag = 0;
  if (post_process_flag.calc_tangent_space) flag |= aiProcess_CalcTangentSpace;
  if (post_process_flag.join_identical_vertices) flag |= aiProcess_JoinIdenticalVertices;
  if (post_process_flag.make_left_handed) flag |= aiProcess_MakeLeftHanded;
  if (post_process_flag.triangulate) flag |= aiProcess_Triangulate;
  if (post_process_flag.remove_component) flag |= aiProcess_RemoveComponent;
  if (post_process_flag.gen_normals) flag |= aiProcess_GenNormals;
  if (post_process_flag.gen_smooth_normals) flag |= aiProcess_GenSmoothNormals;
  if (post_process_flag.split_large_meshes) flag |= aiProcess_SplitLargeMeshes;
  if (post_process_flag.pre_transform_vertices) flag |= aiProcess_PreTransformVertices;
  if (post_process_flag.limit_bone_weights) flag |= aiProcess_LimitBoneWeights;
  if (post_process_flag.validate_data_structure) flag |= aiProcess_ValidateDataStructure;
  if (post_process_flag.improve_cache_locality) flag |= aiProcess_ImproveCacheLocality;
  if (post_process_flag.remove_redundant_materials) flag |= aiProcess_RemoveRedundantMaterials;
  if (post_process_flag.fix_infacing_normals) flag |= aiProcess_FixInfacingNormals;
  if (post_process_flag.sort_by_ptype) flag |= aiProcess_SortByPType;
  if (post_process_flag.find_degenerates) flag |= aiProcess_FindDegenerates;
  if (post_process_flag.find_invalid_data) flag |= aiProcess_FindInvalidData;
  if (post_process_flag.gen_uv_coords) flag |= aiProcess_GenUVCoords;
  if (post_process_flag.transform_uv_coords) flag |= aiProcess_TransformUVCoords;
  if (post_process_flag.find_instances) flag |= aiProcess_FindInstances;
  if (post_process_flag.optimize_meshes) flag |= aiProcess_OptimizeMeshes;
  if (post_process_flag.optimize_graph) flag |= aiProcess_OptimizeGraph;
  if (post_process_flag.flip_uvs) flag |= aiProcess_FlipUVs;
  if (post_process_flag.flip_winding_order) flag |= aiProcess_FlipWindingOrder;
  if (post_process_flag.split_by_bone_count) flag |= aiProcess_SplitByBoneCount;
  if (post_process_flag.debone) flag |= aiProcess_Debone;
  return flag;
}

uint32 MeshConverter::GetRemoveComponentFlag() {
  uint32 flag = 0;
  if (remove_component_flag.normals) flag |= aiComponent_NORMALS;
  if (remove_component_flag.tangents_and_bitangents) flag |= aiComponent_TANGENTS_AND_BITANGENTS;
  if (remove_component_flag.colors) flag |= aiComponent_COLORS;
  if (remove_component_flag.colors0) flag |= aiComponent_COLORSn(0);
  if (remove_component_flag.colors1) flag |= aiComponent_COLORSn(1);
  if (remove_component_flag.colors2) flag |= aiComponent_COLORSn(2);
  if (remove_component_flag.colors3) flag |= aiComponent_COLORSn(3);
  if (remove_component_flag.texcoords) flag |= aiComponent_TEXCOORDS;
  if (remove_component_flag.texcoords0) flag |= aiComponent_TEXCOORDSn(0);
  if (remove_component_flag.texcoords1) flag |= aiComponent_TEXCOORDSn(1);
  if (remove_component_flag.texcoords2) flag |= aiComponent_TEXCOORDSn(2);
  if (remove_component_flag.texcoords3) flag |= aiComponent_TEXCOORDSn(3);
  if (remove_component_flag.boneweights) flag |= aiComponent_BONEWEIGHTS;
  if (remove_component_flag.animations) flag |= aiComponent_ANIMATIONS;
  if (remove_component_flag.textures) flag |= aiComponent_TEXTURES;
  if (remove_component_flag.lights) flag |= aiComponent_LIGHTS;
  if (remove_component_flag.cameras) flag |= aiComponent_CAMERAS;
  if (remove_component_flag.meshes) flag |= aiComponent_MESHES;
  if (remove_component_flag.materials) flag |= aiComponent_MATERIALS;
  return flag;
}

uint32 MeshConverter::GetRemoveTopologyFlag() {
  uint32 flag = 0;
  if (remove_topology_flag.point) flag |= aiPrimitiveType_POINT;
  if (remove_topology_flag.line) flag |= aiPrimitiveType_LINE;
  if (remove_topology_flag.triangle) flag |= aiPrimitiveType_TRIANGLE;
  if (remove_topology_flag.polygon) flag |= aiPrimitiveType_POLYGON;
  return flag;
}

} // namespace xEngine
