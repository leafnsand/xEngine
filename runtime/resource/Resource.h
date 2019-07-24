#ifndef XENGINE_RESOURCE_RESOURCE_H
#define XENGINE_RESOURCE_RESOURCE_H

#include "core/Types.h"

namespace xEngine {

using ResourceID = uint64;

using ResourcePoolID = uint16;

using ResourceSlotID = uint16;

using ResourceUniqueID = uint32;

enum: ResourceID {
  kInvalidResourceID = 0xffffffffffffffff,
};

enum: ResourcePoolID {
  kInvalidResourcePoolID = 0xffff,
  kResourcePoolIDMask = 0xffff,
  kResourcePoolIDOffset = 32,
};

enum: ResourceSlotID {
  kInvalidResourceSlotID = 0xffff,
  kResourceSlotIDMask = 0xffff,
  kResourceSlotIDOffset = 16,
};

enum: ResourceUniqueID {
  kInvalidResourceUniqueID = 0xffffffff,
  kResourceUniqueIDMask = 0xffffffff,
  kResourceUniqueIDOffset = 0,
};

enum class ResourceStatus: uint8 {
  kInvalid,
  kPending,
  kLoading,
  kCompleted,
  kFailed,
};

inline ResourceID GenerateResourceID(ResourceUniqueID uniqueID, ResourceSlotID slotID, ResourcePoolID poolID) {
  return (static_cast<ResourceID>(poolID) & kResourcePoolIDMask) << kResourcePoolIDOffset
      | (static_cast<ResourceID>(slotID) & kResourceSlotIDMask) << kResourceSlotIDOffset
      | (static_cast<ResourceID>(uniqueID) & kResourceUniqueIDMask) << kResourceUniqueIDOffset;
}

inline ResourcePoolID GetResourcePoolIDOfResourceID(ResourceID rid) {
  return static_cast<ResourcePoolID>(rid >> kResourcePoolIDOffset & kResourcePoolIDMask);
}

inline ResourceSlotID GetSlotIDOfResourceID(ResourceID rid) {
  return static_cast<ResourceSlotID>(rid >> kResourceSlotIDOffset & kResourceSlotIDMask);
}

inline ResourceUniqueID GetUniqueIDOfResourceID(ResourceID rid) {
  return static_cast<ResourceUniqueID>(rid >> kResourceUniqueIDOffset & kResourceUniqueIDMask);
}

inline ResourcePoolID GenerateResourcePoolID() {
  static ResourcePoolID last_resource_pool_id = kInvalidResourcePoolID;
  return ++last_resource_pool_id;
}

template<class CONFIG>
class Resource {
 public:
  virtual ~Resource() {}

  void Initialize(ResourceID id, const CONFIG &config) {
    id_ = id;
    status_ = ResourceStatus::kPending;
    config_ = config;
  }

  void Finalize() {
    id_ = kInvalidResourceID;
    status_ = ResourceStatus::kInvalid;
    config_ = CONFIG();
    Reset();
  }

  virtual void Reset() = 0;

  void Loading() {
    x_assert(status_ == ResourceStatus::kPending);
    status_ = ResourceStatus::kLoading;
  }

  void Complete() {
    x_assert(status_ == ResourceStatus::kLoading);
    status_ = ResourceStatus::kCompleted;
  }

  void Failed() {
    x_assert(status_ == ResourceStatus::kLoading);
    status_ = ResourceStatus::kFailed;
  }

  ResourceID id() const { return id_; }

  ResourceStatus status() const { return status_; }

  CONFIG &config() { return config_; }

 private:
  ResourceID id_{kInvalidResourceID};
  ResourceStatus status_{ResourceStatus::kInvalid};
  CONFIG config_;
};

} // namespace xEngine

#endif // XENGINE_RESOURCE_RESOURCE_H