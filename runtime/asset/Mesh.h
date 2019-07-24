#ifndef XENGINE_ASSET_MESH_H
#define XENGINE_ASSET_MESH_H

#include "core/Core.h"
#include "graphics/Graphics.h"
#include "resource/Resource.h"
#include "util/MeshUtil.h"

namespace xEngine {

class Mesh;

PTR_DECLARE(Mesh)

class Mesh {
 public:
  static MeshPtr Parse(ResourceID window, DataPtr data);

  static MeshPtr Parse(ResourceID window, MeshUtil util);

 public:
  CREATE_FUNC_DECLARE(Mesh)

  void Initialize();

  void Finalize();

  void Apply();

 private:
  MeshConfig config_;
  ResourceID window_id_{kInvalidResourceID};
  ResourceID mesh_id_{kInvalidResourceID};
  DrawCallState draw_call_state_;
};

template <typename T>
T read(void **pointer) {
  auto cast_pointer = static_cast<T *>(*pointer);
  auto value = *cast_pointer++;
  *pointer = cast_pointer;
  return value;
}

} // namespace xEngine

#endif // XENGINE_ASSET_MESH_H