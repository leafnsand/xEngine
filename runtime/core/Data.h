#ifndef XENGINE_CORE_DATA_H
#define XENGINE_CORE_DATA_H

#include "Core.h"

#include <EASTL/shared_ptr.h>

namespace xEngine {

class Data {
 public:
  CREATE_FUNC_DECLARE(Data)

  Data();

  explicit Data(size_t size);

  Data(const void *buffer, size_t size);

  Data(Data &other);

  Data(Data &&other) noexcept;

  ~Data();

  void operator=(Data &other);

  void operator=(Data &&other) noexcept;

  Data SubData(size_t offset, size_t size);

  size_t size() const { return size_ ;}

  void *buffer();

 private:
  struct Base;
  Base *base_{nullptr};
  size_t offset_{0};
  size_t size_{0};
};

PTR_DECLARE(Data)

} // namespace xEngine

#endif //XENGINE_CORE_DATA_H
