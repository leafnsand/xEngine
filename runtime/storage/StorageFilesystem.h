#ifndef XENGINE_STORAGE_STORAGEFILESYSTEM_H
#define XENGINE_STORAGE_STORAGEFILESYSTEM_H

#include "io/Filesystem.h"

namespace xEngine {

class StorageFilesystem: public Filesystem {
 public:
  FS_CREATOR_DECLARE(StorageFilesystem)

  virtual DataPtr Read(Location location) override;

  virtual void Write(Location location, DataPtr data) override;

  virtual IOStatus last_status() const override { return last_status_; }

 private:
  IOStatus last_status_{IOStatus::kFailed};
};

}

#endif // XENGINE_STORAGE_STORAGEFILESYSTEM_H