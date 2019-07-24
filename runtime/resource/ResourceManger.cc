#include "ResourceManager.h"

#include <EASTL/algorithm.h>

namespace xEngine {

void ResourceManager::Add(const ResourceIdentity &identity, ResourceID id) {
  resource_id_cache_.push_back(id);
  if (!identity.IsUnique()) {
    identity_to_id_.insert({identity, id});
    id_to_identity_.insert({id, identity});
  }
}

void ResourceManager::Remove(ResourceID id) {
  const auto &cache = eastl::find(resource_id_cache_.begin(), resource_id_cache_.end(), id);
  if (cache != resource_id_cache_.end()) {
    resource_id_cache_.erase(cache);
    const auto &node = id_to_identity_.find(id);
    if (node != id_to_identity_.end()) {
      identity_to_id_.erase(node->second);
      id_to_identity_.erase(id);
    }
  }
}

void ResourceManager::RemoveAll() {
  resource_id_cache_.clear();
  identity_to_id_.clear();
  id_to_identity_.clear();
}

ResourceID ResourceManager::Find(const ResourceIdentity &identity) const {
  ResourceID id = kInvalidResourceID;
  if (!identity.IsUnique()) {
    const auto &node = identity_to_id_.find(identity);
    if (node != identity_to_id_.end()) {
      id = node->second;
    }
  }
  return id;
}

bool ResourceManager::Contains(ResourceID id) const {
  return eastl::find(resource_id_cache_.begin(), resource_id_cache_.end(), id) != resource_id_cache_.end();
}

} // namespace xEngine