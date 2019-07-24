#include "IO.h"

#include "core/Core.h"

namespace xEngine {

void IO::Initialize(int32 threadCount) {
  x_assert(!Available());
  for (auto i = 0; i < threadCount; ++i) {
    auto worker = IOWorker::Create();
    worker->Initialize();
    workers_.push_back(worker);
  }
  Application::GetInstance().AddLoopDelegate(this);
  available_ = true;
}

void IO::Finalize() {
  x_assert(Available());
  for (auto &worker : workers_) {
    worker->Finalize();
  }
  Application::GetInstance().RemoveLoopDelegate(this);
  available_ = false;
}

void IO::Read(const eastl::string &file, IOCallbackFunction callback) {
  auto message = IOReadMessage::Create(location_placeholder_manager_.ResolveLocation(file), callback);
  workers_[++last_worker_ % workers_.size()]->Handle(message);
}

void IO::Write(const eastl::string &file, DataPtr data, IOCallbackFunction callback) {
  auto message = IOWriteMessage::Create(location_placeholder_manager_.ResolveLocation(file), data, callback);
  workers_[++last_worker_ % workers_.size()]->Handle(message);
}

void IO::OnAfterEventTick() {
  for (auto &worker : workers_) {
    worker->Tick();
  }
}

}