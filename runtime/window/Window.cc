#include "Window.h"

namespace xEngine {

static const ResourcePoolID g_window_pool_resource_id = GenerateResourcePoolID();

void Window::Initialize(uint16 pool_size) {
  x_assert(!Available());
  pool_.Initialize(pool_size, g_window_pool_resource_id);
  available_ = true;
}

void Window::Finalize() {
  available_ = false;
  for (auto &id : resource_id_cache_) {
    auto &resource = pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      factory_.Destroy(resource);
    }
    pool_.Destroy(id);
  }
  pool_.Finalize();
  RemoveAll();
}

bool Window::Available() const {
  return available_;
}

ResourceID Window::Create(const WindowConfig &config) {
  x_assert(Available());
  auto id = pool_.Create(config);
  Add(config.identity, id);
  auto &resource = pool_.Find(id);
  factory_.Create(resource);
  return id;
}

void Window::Destroy(ResourceID id) {
  x_assert(Available());
  x_assert(Contains(id));
  Remove(id);
  auto &resource = pool_.Find(id);
  if (resource.status() != ResourceStatus::kInvalid) {
    factory_.Destroy(resource);
  }
  pool_.Destroy(id);
}

const eastl::unique_ptr<WindowInterface> &Window::Get(ResourceID id) {
  x_assert(Available());

  static const eastl::unique_ptr<WindowInterface> invalid = nullptr;

  auto &resource = pool_.Find(id);
  if (resource.status() != ResourceStatus::kInvalid) {
    return resource.window;
  }
  return invalid;
}

const eastl::unique_ptr<Graphics> &Window::GetGraphics(ResourceID id) {
  x_assert(Available());

  static const eastl::unique_ptr<Graphics> invalid = nullptr;

  auto &resource = pool_.Find(id);
  if (resource.status() != ResourceStatus::kInvalid) {
    return resource.window->graphics();
  }
  return invalid;
}

}