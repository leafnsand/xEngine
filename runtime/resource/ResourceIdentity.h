#ifndef XENGINE_RESOURCE_RESOURCENAME_H
#define XENGINE_RESOURCE_RESOURCENAME_H

#include "core/Core.h"
#include "core/Types.h"

#include <EASTL/string.h>
#include <EASTL/hash_map.h>

namespace xEngine {

using ResourceSignature = uint32;

enum: ResourceSignature {
  kDefaultSignature = 0x7fffffff,
  kUniqueSignature = 0xffffffff,
};

inline ResourceSignature GenerateResourceSignature() {
  static ResourceSignature last_resource_signature = kUniqueSignature;
  return ++last_resource_signature;
}

class ResourceIdentity {
 public:
  static ResourceIdentity Unique() {
    ResourceIdentity instance;
    instance.signature_ = kUniqueSignature;
    return instance;
  }

  static ResourceIdentity Unique(const eastl::string &name) {
    ResourceIdentity instance;
    instance.name_ = name;
    instance.signature_ = kUniqueSignature;
    return instance;
  }

  static ResourceIdentity Unique(const char *name) {
    ResourceIdentity instance;
    instance.name_ = name;
    instance.signature_ = kUniqueSignature;
    return instance;
  }

  static ResourceIdentity Shared(ResourceSignature signature = kDefaultSignature) {
    x_assert(signature != kUniqueSignature);
    ResourceIdentity instance;
    instance.signature_ = signature;
    return instance;
  }

  static ResourceIdentity Shared(const eastl::string &name, ResourceSignature signature = kDefaultSignature) {
    x_assert(signature != kUniqueSignature);
    ResourceIdentity instance;
    instance.name_ = name;
    instance.signature_ = signature;
    return instance;
  }

  static ResourceIdentity Shared(const char *name, ResourceSignature signature = kDefaultSignature) {
    x_assert(signature != kUniqueSignature);
    ResourceIdentity instance;
    instance.name_ = name;
    instance.signature_ = signature;
    return instance;
  }

  void operator=(const ResourceIdentity &other) {
    name_ = other.name_;
    signature_ = other.signature_;
  }

  bool IsUnique() const {
    return signature_ == kUniqueSignature;
  }

  bool operator<(const ResourceIdentity &other) const {
    return signature_ < other.signature_ || name_ < other.name_;
  }

  bool operator==(const ResourceIdentity &other) const {
    return signature_ == other.signature_ && name_ == other.name_;
  }

  bool operator!=(const ResourceIdentity &other) const {
    return signature_ != other.signature_ || name_ != other.name_;
  }

  const eastl::string &name() const { return name_; }

  size_t Hash() const { return eastl::hash<eastl::string>()(name_) ^ eastl::hash<int32>()(signature_); }

 private:
  eastl::string name_;
  ResourceSignature signature_{kDefaultSignature};
};

} // namespace xEngine

namespace eastl {

template<>
struct hash<xEngine::ResourceIdentity> {
  size_t operator()(const xEngine::ResourceIdentity &other) const { return other.Hash(); }
};

} // namespace eastl

#endif // XENGINE_RESOURCE_RESOURCENAME_H