#ifndef XENGINE_RESOURCE_RESOURCEPOOL_H
#define XENGINE_RESOURCE_RESOURCEPOOL_H

#include "Resource.h"

#include "core/Core.h"

#include <EASTL/queue.h>
#include <EASTL/vector.h>

namespace xEngine {

template<class RESOURCE, class CONFIG>
class ResourcePool {
 public:
  virtual ~ResourcePool() { x_assert(pool_id_ == kInvalidResourcePoolID); }

  void Initialize(uint16 size, ResourcePoolID pool_id);

  void Finalize();

  ResourceID Create(const CONFIG &config);

  void Destroy(ResourceID id);

  RESOURCE &Find(ResourceID id);

  ResourcePoolID pool_id() const { return pool_id_; }

 public:
  static const RESOURCE &invalid() { return invalid_; }

 private:
  static RESOURCE invalid_;

 protected:
  ResourceUniqueID next_unique_id_{0};
  ResourcePoolID pool_id_{kInvalidResourcePoolID};
  eastl::vector<RESOURCE> slots_;
  eastl::queue<uint16> available_slots_;
};

template<class RESOURCE, class CONFIG>
RESOURCE ResourcePool<RESOURCE, CONFIG>::invalid_;

template<class RESOURCE, class CONFIG>
void ResourcePool<RESOURCE, CONFIG>::Initialize(uint16 size, ResourcePoolID pool_id) {
  x_assert(pool_id_ == kInvalidResourcePoolID);
  pool_id_ = pool_id;
  slots_.resize(size);
  for (auto i = 0; i < size; ++i) {
    available_slots_.push(static_cast<uint16>(i));
  }
}

template<class RESOURCE, class CONFIG>
void ResourcePool<RESOURCE, CONFIG>::Finalize() {
  x_assert(pool_id_ != kInvalidResourcePoolID);
  pool_id_ = kInvalidResourcePoolID;
  slots_.clear();
  while (!available_slots_.empty()) {
    available_slots_.pop();
  }
}

template<class RESOURCE, class CONFIG>
ResourceID ResourcePool<RESOURCE, CONFIG>::Create(const CONFIG &config) {
  x_assert(pool_id_ != kInvalidResourcePoolID && !available_slots_.empty());
  auto slotID = available_slots_.front();
  available_slots_.pop();
  auto id = GenerateResourceID(next_unique_id_++, slotID, pool_id_);
  auto &slot = slots_[GetSlotIDOfResourceID(id)];
  x_assert(slot.status() == ResourceStatus::kInvalid);
  slot.Initialize(id, config);
  return id;
}

template<class RESOURCE, class CONFIG>
void ResourcePool<RESOURCE, CONFIG>::Destroy(ResourceID id) {
  x_assert(pool_id_ != kInvalidResourcePoolID);
  auto &slot = slots_[GetSlotIDOfResourceID(id)];
  x_assert(slot.id() == id);
  x_assert(slot.status() != ResourceStatus::kInvalid);
  slot.Finalize();
  available_slots_.push(GetSlotIDOfResourceID(id));
}

template<class RESOURCE, class CONFIG>
RESOURCE &ResourcePool<RESOURCE, CONFIG>::Find(ResourceID id) {
  x_assert(pool_id_ != kInvalidResourcePoolID);
  if (GetResourcePoolIDOfResourceID(id) == pool_id_) {
    auto &slot = slots_[GetSlotIDOfResourceID(id)];
    if (slot.status() != ResourceStatus::kInvalid) {
      return slot;
    }
  }
  return invalid_;
}

} // namespace xEngine

#endif // XENGINE_RESOURCE_RESOURCEPOOL_H