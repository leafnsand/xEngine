#ifndef XENGINE_IO_IOMESSAGE_H
#define XENGINE_IO_IOMESSAGE_H

#include "Location.h"

#include "core/Core.h"
#include "core/Data.h"

#include <EASTL/string.h>
#include <EASTL/shared_ptr.h>

namespace xEngine {

enum class IOStatus {
  kSuccess,
  kFailed,
};

using IOCallbackFunction = eastl::function<void(Location, IOStatus, DataPtr)>;

enum class IOMessageType {
  kInvalid,
  kRead,
  kWrite,
  kRegisterFilesystem,
};

class IOMessage {
 public:
  IOMessage(IOMessageType type) : type_(type) {}

  IOMessageType type() const { return type_; }

 private:
  IOMessageType type_{IOMessageType::kInvalid};
};

PTR_DECLARE(IOMessage)

class IOReadMessage: public IOMessage {
 public:
  CREATE_FUNC_DECLARE(IOReadMessage)

  IOReadMessage(Location location, IOCallbackFunction callback) :
      IOMessage(IOMessageType::kRead),
      location_(location),
      callback_(callback) {}

  void Notify() { if (callback_) callback_(location_, status_, data_); }

  void set_status(IOStatus value) { status_ = value; }

  IOStatus status() const { return status_; }

  const Location &location() const { return location_; }

  DataPtr data() const { return data_; }

  void set_data(DataPtr value) { data_ = value; }

 private:
  IOStatus status_{IOStatus::kFailed};
  Location location_;
  DataPtr data_;
  IOCallbackFunction callback_;
};

PTR_DECLARE(IOReadMessage)

class IOWriteMessage: public IOMessage {
 public:
  CREATE_FUNC_DECLARE(IOWriteMessage)

  IOWriteMessage(Location location, DataPtr data, IOCallbackFunction callback) :
      IOMessage(IOMessageType::kWrite),
      location_(location),
      data_(data),
      callback_(callback) {}

  void Notify() { if (callback_) callback_(location_, status_, data_); }

  void set_status(IOStatus value) { status_ = value; }

  IOStatus status() const { return status_; }

  Location location() const { return location_; }

  DataPtr data() const { return data_; }

 private:
  IOStatus status_{IOStatus::kFailed};
  Location location_;
  DataPtr data_;
  IOCallbackFunction callback_;
};

PTR_DECLARE(IOWriteMessage)

class IORegisterFilesystemMessage: public IOMessage {
 public:
  CREATE_FUNC_DECLARE(IORegisterFilesystemMessage)

  IORegisterFilesystemMessage(eastl::string name) : IOMessage(IOMessageType::kRegisterFilesystem), name_(name) {}

  const eastl::string &name() const { return name_; }

 private:
  eastl::string name_;
};

PTR_DECLARE(IORegisterFilesystemMessage)

}

#endif // XENGINE_IO_IOMESSAGE_H