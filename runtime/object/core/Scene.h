#ifndef XENGINE_OBJECT_CORE_SCENE_H
#define XENGINE_OBJECT_CORE_SCENE_H

#include "GameObject.h"

#include <EASTL/list.h>

namespace xEngine {

class Scene {
 public:
  static Scene &GetInstance() {
    static Scene instance;
    return instance;
  }

 public:
  void Initialize();

  void Finalize();

  GameObjectPtr AddGameObject();

  void RemoveGameObject(GameObjectPtr game_object);

  GameObjectPtr FindGameObject(uint32 id);

  void Update();

  void Serialize();

  void Deserialize();

 private:
  eastl::list<GameObjectPtr> game_objects_;
};

} // namespace xEngine

#endif // XENGINE_OBJECT_CORE_SCENE_H
