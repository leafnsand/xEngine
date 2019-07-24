#ifndef XENGINE_IO_FILESYSTEMMANAGER_H
#define XENGINE_IO_FILESYSTEMMANAGER_H

#include <EASTL/hash_map.h>
#include <EASTL/string.h>

namespace xEngine {

class FilesystemManager {
 public:
  void RegisterFilesystem(const eastl::string &name, Filesystem::Creator function) {
    filesystem_creators_[name] = function;
  }

  FilesystemPtr CreateFilesystem(const eastl::string &name) {
    const auto &iterator = filesystem_creators_.find(name);
    if (iterator != filesystem_creators_.end() && iterator->second != nullptr) {
      return iterator->second();
    }
    return nullptr;
  }

 private:
  eastl::hash_map<eastl::string, Filesystem::Creator> filesystem_creators_;
};

}

#endif // XENGINE_IO_FILESYSTEMMANAGER_H