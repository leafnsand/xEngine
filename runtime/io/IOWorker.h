#ifndef XENGINE_IO_IOWORKER_H
#define XENGINE_IO_IOWORKER_H

#include "IOMessage.h"
#include "Filesystem.h"

#include <EASTL/hash_map.h>
#include <EASTL/queue.h>
#include <EASTL/string.h>

#include <atomic>
#include <mutex>
#include <thread>

namespace xEngine {

class IOWorker {
 public:
  CREATE_FUNC_DECLARE(IOWorker)

  void Initialize();

  void Finalize();

  void Handle(IOMessagePtr message);

  void Tick();

 private:
  void HandleWaitingMessage();

  void HandleComingMessage();

  void NotifyMessage();

 private:
#if X_HAS_THREADS
  std::thread thread_;
  std::atomic_bool running_{false};
  std::mutex waiting_queue_lock_;
  std::mutex forwarding_queue_lock_;
  std::condition_variable condition_variable_;
#else
  bool running_{false};
#endif
  eastl::queue<IOMessagePtr> coming_messages_;
  eastl::queue<IOMessagePtr> waiting_messages_;
  eastl::queue<IOMessagePtr> pending_messages_;
  eastl::queue<IOMessagePtr> done_messages_;
  eastl::queue<IOMessagePtr> forwarding_messages_;
  eastl::queue<IOMessagePtr> notify_messages_;
  eastl::hash_map<eastl::string, FilesystemPtr> filesystems_;
};

PTR_DECLARE(IOWorker)

}

#endif // XENGINE_IO_IOWORKER_H