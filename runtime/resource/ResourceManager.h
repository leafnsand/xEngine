#ifndef XENGINE_RESOURCE_RESOURCEMANAGER_H
#define XENGINE_RESOURCE_RESOURCEMANAGER_H

#include "Resource.h"
#include "ResourceIdentity.h"

#include <EASTL/hash_map.h>
#include <EASTL/vector.h>

namespace xEngine {

class ResourceManager {
 protected:
  virtual ~ResourceManager() {}

  void Add(const ResourceIdentity &identity, ResourceID id);

  void Remove(ResourceID id);

  void RemoveAll();

  ResourceID Find(const ResourceIdentity &identity) const;

  bool Contains(ResourceID id) const;

  size_t Size() const { return resource_id_cache_.size(); }

 protected:
  eastl::vector<ResourceID> resource_id_cache_;
  eastl::hash_map<ResourceIdentity, ResourceID> identity_to_id_;
  eastl::hash_map<ResourceID, ResourceIdentity> id_to_identity_;
};

} // namespace xEngine

#endif // XENGINE_RESOURCE_RESOURCEMANAGER_H