#include "core/Data.h"

#include <EASTL/allocator.h>

namespace xEngine {

struct Data::Base {
  Base() {}
  Base(size_t size) : size(size) {
    if (size > 0) {
      buffer = eastl::GetDefaultAllocator()->allocate(size);
    }
  }
  ~Base() {
    if (buffer != nullptr) {
      eastl::GetDefaultAllocator()->deallocate(buffer, size);
    }
  }
  void Retain() {
    ++ref;
  }
  void Release() {
    if (--ref == 0) {
      delete this;
    }
  }
  size_t ref{0};
  size_t size{0};
  void *buffer{nullptr};
};

Data::Data() {
  base_ = new Base;
  base_->Retain();
}

Data::Data(size_t size) {
  base_ = new Base(size);
  base_->Retain();
  size_ = size;
}

Data::Data(const void *buffer, size_t size) {
  base_ = new Base(size);
  base_->Retain();
  size_ = size;
  memcpy(base_->buffer, buffer, size);
}

Data::Data(Data &other) {
  base_ = other.base_;
  offset_ = other.offset_;
  size_ = other.size_;
}

Data::Data(Data &&other) noexcept {
  base_ = other.base_;
  offset_ = other.offset_;
  size_ = other.size_;
  other.base_ = nullptr;
  other.offset_ = 0;
  other.size_ = 0;
}

Data::~Data() {
  base_->Release();
}

void Data::operator=(Data &other) {
  base_ = other.base_;
  offset_ = other.offset_;
  size_ = other.size_;
}

void Data::operator=(Data &&other) noexcept {
  base_ = other.base_;
  offset_ = other.offset_;
  size_ = other.size_;
  other.base_ = nullptr;
  other.offset_ = 0;
  other.size_ = 0;
}

Data Data::SubData(size_t offset, size_t size) {
  Data data;
  base_->Retain();
  data.base_ = base_;
  data.offset_ = eastl::min(offset, base_->size);
  data.size_ = eastl::min(size, base_->size - data.offset_);
  return data;
}

void *Data::buffer() {
  return static_cast<uint8 *>(base_->buffer) + offset_;
}

} // namespace xEngine