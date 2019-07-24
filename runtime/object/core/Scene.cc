#include "Scene.h"

namespace xEngine {

void Scene::Initialize() {

}

void Scene::Finalize() {
  game_objects_.clear();
}

GameObjectPtr Scene::AddGameObject() {
  auto game_object = GameObject::Create();
  game_objects_.push_back(game_object);
  return game_object;
}

void Scene::RemoveGameObject(GameObjectPtr game_object) {
  if (eastl::find(game_objects_.begin(), game_objects_.end(), game_object) == game_objects_.end()) return;
  game_objects_.remove(game_object);
}

GameObjectPtr Scene::FindGameObject(uint32 id) {
  for (auto &game_object : game_objects_) {
    if (game_object->id() == id) {
      return game_object;
    }
  }
  return nullptr;
}

void Scene::Update() {
  for (auto &game_object : game_objects_) {
    game_object->Update();
  }
}

void Scene::Serialize() {

}

void Scene::Deserialize() {

}

} // namespace xEngine
