#ifndef XENGINE_HTTP_HTTPFILESYSTEM_H
#define XENGINE_HTTP_HTTPFILESYSTEM_H

#include "io/Filesystem.h"

#include <mongoose.h>

namespace xEngine {

class HTTPFilesystem: public Filesystem {
 public:
  FS_CREATOR_DECLARE(HTTPFilesystem)

  HTTPFilesystem();

  ~HTTPFilesystem();

  virtual DataPtr Read(Location location) override;

  virtual void Write(Location location, DataPtr data) override;

  virtual IOStatus last_status() const override { return last_status_; }

 private:
  static void Handle(mg_connection *connection, int event, void *eventData);

 private:
  bool polling_{false};
  DataPtr data_;
  IOStatus last_status_{IOStatus::kFailed};
  mg_mgr context_;
};

}

#endif // XENGINE_HTTP_HTTPFILESYSTEM_H