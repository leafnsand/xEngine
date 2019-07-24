#ifndef XENGINE_IO_LOCATIONPLACEHOLDERMANAGER_H
#define XENGINE_IO_LOCATIONPLACEHOLDERMANAGER_H

#include "Location.h"

#include "core/Core.h"

#include <EASTL/string.h>
#include <EASTL/hash_map.h>

namespace xEngine {

class LocationPlaceholderManager {
 public:
  void AddPlaceholder(const eastl::string &replacement, const eastl::string &original) {
    placeholders_[replacement] = original;
  }

  void RemovePlaceholder(const eastl::string &replacement) {
    if (HasPlaceholder(replacement)) placeholders_.erase(replacement);
  }

  bool HasPlaceholder(const eastl::string &replacement) {
    return placeholders_.find(replacement) != placeholders_.end();
  }

  Location ResolveLocation(const eastl::string &path) {
    auto location = path;
    for (auto &pair : placeholders_) {
      if (location.find(pair.first) == 0) {
        location.replace(0, pair.first.length() + 1, pair.second);
        break;
      }
    }
    return location;
  }

 private:
  eastl::hash_map<eastl::string, eastl::string> placeholders_;
};

}

#endif // XENGINE_IO_LOCATIONPLACEHOLDERMANAGER_H