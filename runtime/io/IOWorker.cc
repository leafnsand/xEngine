#include "IOWorker.h"
#include "IO.h"

#include "core/Core.h"

namespace xEngine {

void IOWorker::Initialize() {
  x_assert(!running_);
  running_ = true;
#if X_HAS_THREADS
  thread_ = std::thread([this](){
    this->HandleWaitingMessage();
  });
#endif
}

void IOWorker::Finalize() {
  x_assert(running_);
  running_ = false;
#if X_HAS_THREADS
  condition_variable_.notify_one();
  thread_.join();
#else
  Tick();
#endif
}

void IOWorker::Handle(IOMessagePtr message) {
  coming_messages_.push(message);
}

void IOWorker::Tick() {
  HandleComingMessage();
#if !X_HAS_THREADS
  HandleWaitingMessage();
#endif
  NotifyMessage();
}

void IOWorker::HandleWaitingMessage() {
#if X_HAS_THREADS
  while (running_) {
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    condition_variable_.wait(lock);
    lock.unlock();
    waiting_queue_lock_.lock();
#endif
  pending_messages_ = std::move(waiting_messages_);
#if X_HAS_THREADS
  waiting_queue_lock_.unlock();
#endif
  while (!pending_messages_.empty()) {
    auto &message = pending_messages_.front();
    switch (message->type()) {
      case IOMessageType::kRead: {
        auto read_message = eastl::static_pointer_cast<IOReadMessage>(message);
        const auto &location = read_message->location();
        auto &filesystem = filesystems_[location.filesystem_name()];
        if (filesystem) {
          read_message->set_data(filesystem->Read(location));
          read_message->set_status(filesystem->last_status());
        }
        done_messages_.push(read_message);
        break;
      }
      case IOMessageType::kWrite: {
        auto write_message = eastl::static_pointer_cast<IOWriteMessage>(message);
        const auto &location = write_message->location();
        auto &filesystem = filesystems_[location.filesystem_name()];
        if (filesystem) {
          filesystem->Write(location, write_message->data());
          write_message->set_status(filesystem->last_status());
        }
        done_messages_.push(write_message);
        break;
      }
      case IOMessageType::kRegisterFilesystem: {
        auto register_filesystem_message = eastl::static_pointer_cast<IORegisterFilesystemMessage>(message);
        filesystems_[register_filesystem_message->name()] =
            IO::GetInstance().CreateFilesystem(register_filesystem_message->name());
        break;
      }
      default:
        break;
    }
    pending_messages_.pop();
  }
  if (!done_messages_.empty()) {
#if X_HAS_THREADS
    forwarding_queue_lock_.lock();
#endif
    if (forwarding_messages_.empty()) {
      forwarding_messages_ = std::move(done_messages_);
    } else {
      while (!done_messages_.empty()) {
        forwarding_messages_.push(done_messages_.front());
        done_messages_.pop();
      }
    }
#if X_HAS_THREADS
    forwarding_queue_lock_.unlock();
#endif
  }
#if X_HAS_THREADS
  }
#endif
}

void IOWorker::HandleComingMessage() {
  if (!coming_messages_.empty()) {
#if X_HAS_THREADS
    waiting_queue_lock_.lock();
#endif
    if (waiting_messages_.empty()) {
      waiting_messages_ = std::move(coming_messages_);
    } else {
      while (!coming_messages_.empty()) {
        waiting_messages_.push(coming_messages_.front());
        coming_messages_.pop();
      }
    }
#if X_HAS_THREADS
    waiting_queue_lock_.unlock();
    condition_variable_.notify_one();
#endif
  }
}

void IOWorker::NotifyMessage() {
#if X_HAS_THREADS
  forwarding_queue_lock_.lock();
#endif
  notify_messages_ = std::move(forwarding_messages_);
#if X_HAS_THREADS
  forwarding_queue_lock_.unlock();
#endif
  while (!notify_messages_.empty()) {
    const auto &message = notify_messages_.front();
    switch (message->type()) {
      case IOMessageType::kRead: {
        auto read_message = eastl::static_pointer_cast<IOReadMessage>(message);
        read_message->Notify();
        break;
      }
      case IOMessageType::kWrite: {
        auto write_message = eastl::static_pointer_cast<IOWriteMessage>(message);
        write_message->Notify();
        break;
      }
      default:
        break;
    }
    notify_messages_.pop();
  }
}

}