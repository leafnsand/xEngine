#ifndef XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTOR_H
#define XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTOR_H

#include "MeshConvertToolDefine.h"

struct aiNode;
struct aiMesh;

namespace xEngine {

class MeshViewerWindow;

class MeshConverter {
 public:
  static void Convert(const eastl::string &in, const eastl::string &out);

 private:
  static void ProcessMeshConfig(MeshUtil &util, aiMesh *mesh);

  static void ProcessMeshVertexPosition(MeshUtil &util, aiMesh *mesh);

  static void ProcessMeshVertexTexcoord(MeshUtil &util, aiMesh *mesh);

  static void ProcessMeshVertexNormal(MeshUtil &util, aiMesh *mesh);

  static void ProcessMeshVertex(MeshUtil &util, aiMesh *mesh);

  static void ProcessMeshIndex(MeshUtil &util, aiMesh *mesh);

  static void ProcessMesh(MeshInfo &info, aiMesh *mesh);

  static void ProcessNode(NodeInfo &info, aiNode *node);

  static void SaveContext(SceneInfo &scene);

  static uint32 GetPostProcessFlag();

  static uint32 GetRemoveComponentFlag();

  static uint32 GetRemoveTopologyFlag();

public:
  static PostProcessFlag post_process_flag;

  static RemoveComponentFlag remove_component_flag;

  static RemoveTopologyFlag remove_topology_flag;

 private:
  static MeshViewerWindow *viewer_;
};

} // namespace xEngine

#endif // XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTOR_H
