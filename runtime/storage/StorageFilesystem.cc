#include "StorageFilesystem.h"

#include <cstdio>

namespace xEngine {

DataPtr StorageFilesystem::Read(Location location) {
  last_status_ = IOStatus::kFailed;
  auto path = location.path();
  auto file = fopen(path.c_str(), "rb");
  if (file) {
    fseek(file, 0, SEEK_SET);
    auto begin = ftell(file);
    fseek(file, 0, SEEK_END);
    auto end = ftell(file);
    fseek(file, 0, SEEK_SET);
    auto data = Data::Create(end - begin);
    fread(data->buffer(), sizeof(char), data->size(), file);
    fclose(file);
    last_status_ = IOStatus::kSuccess;
    return data;
  }
  return nullptr;
}

void StorageFilesystem::Write(Location location, DataPtr data) {
  last_status_ = IOStatus::kFailed;
  auto path = location.path();
  auto file = fopen(path.c_str(), "wb");
  if (file) {
    if (data && data->size() > 0) {
      fwrite(data->buffer(), sizeof(char), data->size(), file);
    }
    fclose(file);
    last_status_ = IOStatus::kSuccess;
  }
}

}