#ifndef XENGINE_IO_IO_H
#define XENGINE_IO_IO_H

#include "IOWorker.h"
#include "Filesystem.h"
#include "FilesystemManager.h"
#include "Location.h"
#include "LocationPlaceholderManager.h"

#include "core/Data.h"
#include "core/Types.h"
#include "application/ApplicationDelegate.h"

#include <EASTL/functional.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

namespace xEngine {

class IO : public ApplicationTickDelegate {
 public:
  static IO &GetInstance() {
    static IO instance;
    return instance;
  }

 public:
  void Initialize(int32 threadCount = 8);

  void Finalize();

  bool Available() const { return available_; }

  void Read(const eastl::string &file, IOCallbackFunction callback);

  void Write(const eastl::string &file, DataPtr data, IOCallbackFunction callback);

  void AddPlaceholder(const eastl::string &replacement, const eastl::string &original) {
    location_placeholder_manager_.AddPlaceholder(replacement, original);
  }

  void RemovePlaceholder(const eastl::string &replacement) {
    location_placeholder_manager_.RemovePlaceholder(replacement);
  }

  bool HasPlaceholder(const eastl::string &replacement) {
    return location_placeholder_manager_.HasPlaceholder(replacement);
  }

  Location ResolveLocation(const eastl::string &path) {
    return location_placeholder_manager_.ResolveLocation(path);
  }

  void RegisterFilesystem(const eastl::string &name, Filesystem::Creator function) {
    filesystem_manager_.RegisterFilesystem(name, function);
    for (auto &worker : workers_) worker->Handle(IORegisterFilesystemMessage::Create(name));
  }

  FilesystemPtr CreateFilesystem(const eastl::string &name) {
    return filesystem_manager_.CreateFilesystem(name);
  }

  virtual void OnAfterEventTick() override;

 private:
  bool available_{false};
  int32 last_worker_{0};
  eastl::vector<IOWorkerPtr> workers_;
  LocationPlaceholderManager location_placeholder_manager_;
  FilesystemManager filesystem_manager_;
};

} // namespace xEngine

#endif // XENGINE_IO_IO_H