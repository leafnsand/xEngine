#ifndef XENGINE_RESOURCE_RESOURCEFACTORY_H
#define XENGINE_RESOURCE_RESOURCEFACTORY_H

#include "Resource.h"

#include "core/Data.h"

namespace xEngine {

template<class RESOURCE>
class ResourceFactory {
 public:
  virtual ~ResourceFactory() {}

  virtual void Create(RESOURCE &resource) = 0;

  virtual void Destroy(RESOURCE &resource) = 0;
};

} // namespace xEngine

#endif // XENGINE_RESOURCE_RESOURCEFACTORY_H