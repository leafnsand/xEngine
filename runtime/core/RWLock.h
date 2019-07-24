#ifndef XENGINE_CORE_RWLOCK_H
#define XENGINE_CORE_RWLOCK_H

#include "core/Config.h"

#if X_HAS_ATOMIC
# include <atomic>
#endif // X_HAS_ATOMIC

namespace xEngine {

class RWLock final {
 public:
  void LockWrite() {
#if X_HAS_ATOMIC
    while (std::atomic_exchange_explicit(&this->write_lock_, true, std::memory_order_acquire)) {}
    while (this->read_count_ > 0) {}
#endif // X_HAS_ATOMIC
  }

  void UnlockWrite() {
#if X_HAS_ATOMIC
    std::atomic_store_explicit(&this->write_lock_, false, std::memory_order_release);
#endif // X_HAS_ATOMIC
  }

  void LockRead() {
#if X_HAS_ATOMIC
    while (this->write_lock_) {}
    ++this->read_count_;
#endif // X_HAS_ATOMIC
  }

  void UnlockRead() {
#if X_HAS_ATOMIC
    --this->read_count_;
#endif // X_HAS_ATOMIC
  }

 private:
#if X_HAS_ATOMIC
  std::atomic<bool> write_lock_{false};
  std::atomic<int> read_count_{0};
#endif // X_HAS_ATOMIC
};

class RWLockWriteScope final {
 public:
  explicit RWLockWriteScope(RWLock &lock) : lock_(lock) { lock_.LockWrite(); }

  ~RWLockWriteScope() { lock_.UnlockWrite(); }

 private:
  RWLock &lock_;
};

class RWLockReadScope final {
 public:
  explicit RWLockReadScope(RWLock &lock) : lock_(lock) { lock_.LockRead(); }

  ~RWLockReadScope() { lock_.UnlockRead(); }

 private:
  RWLock &lock_;
};

} // namespace xEngine

#endif // XENGINE_CORE_RWLOCK_H
