#if X_OPENGL

#include "OpenGLGraphicsResourceManager.h"

#include "core/Log.h"

namespace xEngine {

static const ResourcePoolID g_opengl_shader_resource_pool_id = GenerateResourcePoolID();

static const ResourcePoolID g_opengl_texture_resource_pool_id = GenerateResourcePoolID();

static const ResourcePoolID g_opengl_mesh_resource_pool_id = GenerateResourcePoolID();

static const ResourcePoolID g_opengl_pipeline_resource_pool_id = GenerateResourcePoolID();

static const ResourcePoolID g_opengl_sampler_resource_pool_id = GenerateResourcePoolID();

static const ResourcePoolID g_opengl_uniform_buffer_resource_pool_id = GenerateResourcePoolID();

void OpenGLGraphicsResourceManager::Initialize(const GraphicsConfig &config) {
  shader_pool_.Initialize(config.shader_pool_size, g_opengl_shader_resource_pool_id);
  texture_pool_.Initialize(config.texture_pool_size, g_opengl_texture_resource_pool_id);
  mesh_pool_.Initialize(config.mesh_pool_size, g_opengl_mesh_resource_pool_id);
  pipeline_pool_.Initialize(config.pipeline_pool_size, g_opengl_pipeline_resource_pool_id);
  sampler_pool_.Initialize(config.sampler_pool_size, g_opengl_sampler_resource_pool_id);
  uniform_buffer_pool_.Initialize(config.uniform_buffer_size, g_opengl_uniform_buffer_resource_pool_id);
}

void OpenGLGraphicsResourceManager::Finalize() {
  for (auto &id : resource_id_cache_) {
    auto pool_id = GetResourcePoolIDOfResourceID(id);
    if (pool_id == g_opengl_shader_resource_pool_id) {
      auto &resource = shader_pool_.Find(id);
      if (resource.status() != ResourceStatus::kInvalid) {
        Log::GetInstance().Warning("Graphics resource (shader) not released: 0x%llx\n", id);
        shader_factory_.Destroy(resource);
      }
      shader_pool_.Destroy(id);
    } else if (pool_id == g_opengl_texture_resource_pool_id) {
      auto &resource = texture_pool_.Find(id);
      if (resource.status() != ResourceStatus::kInvalid) {
        Log::GetInstance().Warning("Graphics resource (texture) not released: 0x%llx\n", id);
        texture_factory_.Destroy(resource);
      }
      texture_pool_.Destroy(id);
    } else if (pool_id == g_opengl_mesh_resource_pool_id) {
      auto &resource = mesh_pool_.Find(id);
      if (resource.status() != ResourceStatus::kInvalid) {
        Log::GetInstance().Warning("Graphics resource (mesh) not released: 0x%llx\n", id);
        mesh_factory_.Destroy(resource);
      }
      mesh_pool_.Destroy(id);
    } else if (pool_id == g_opengl_pipeline_resource_pool_id) {
      auto &resource = pipeline_pool_.Find(id);
      if (resource.status() != ResourceStatus::kInvalid) {
        Log::GetInstance().Warning("Graphics resource (pipeline) not released: 0x%llx\n", id);
        pipeline_factory_.Destroy(resource);
      }
      pipeline_pool_.Destroy(id);
    } else if (pool_id == g_opengl_sampler_resource_pool_id) {
      auto &resource = sampler_pool_.Find(id);
      if (resource.status() != ResourceStatus::kInvalid) {
        Log::GetInstance().Warning("Graphics resource (sampler) not released: 0x%llx\n", id);
        sampler_factory_.Destroy(resource);
      }
      sampler_pool_.Destroy(id);
    } else if (pool_id == g_opengl_uniform_buffer_resource_pool_id) {
      auto &resource = uniform_buffer_pool_.Find(id);
      if (resource.status() != ResourceStatus::kInvalid) {
        Log::GetInstance().Warning("Graphics resource (uniform buffer) not released: 0x%llx\n", id);
        uniform_buffer_factory_.Destroy(resource);
      }
      uniform_buffer_pool_.Destroy(id);
    }
  }
  shader_pool_.Finalize();
  texture_pool_.Finalize();
  mesh_pool_.Finalize();
  pipeline_pool_.Finalize();
  sampler_pool_.Finalize();
  uniform_buffer_pool_.Finalize();
  RemoveAll();
}

ResourceID OpenGLGraphicsResourceManager::Create(const ShaderConfig &config) {
  auto id = shader_pool_.Create(config);
  Add(config.identity, id);
  auto &resource = shader_pool_.Find(id);
  shader_factory_.Create(resource);
  return id;
}

ResourceID OpenGLGraphicsResourceManager::Create(const TextureConfig &config) {
  auto id = texture_pool_.Create(config);
  Add(config.identity, id);
  auto &resource = texture_pool_.Find(id);
  texture_factory_.Create(resource);
  return id;
}

ResourceID OpenGLGraphicsResourceManager::Create(const MeshConfig &config) {
  auto id = mesh_pool_.Create(config);
  Add(config.identity, id);
  auto &resource = mesh_pool_.Find(id);
  mesh_factory_.Create(resource);
  return id;
}

ResourceID OpenGLGraphicsResourceManager::Create(const PipelineConfig &config) {
  auto id = pipeline_pool_.Create(config);
  Add(config.identity, id);
  auto &resource = pipeline_pool_.Find(id);
  pipeline_factory_.Create(resource);
  return id;
}

ResourceID OpenGLGraphicsResourceManager::Create(const SamplerConfig &config) {
  auto id = sampler_pool_.Create(config);
  Add(config.identity, id);
  auto &resource = sampler_pool_.Find(id);
  sampler_factory_.Create(resource);
  return id;
}

ResourceID OpenGLGraphicsResourceManager::Create(const UniformBufferConfig &config) {
  auto id = uniform_buffer_pool_.Create(config);
  Add(config.identity, id);
  auto &resource = uniform_buffer_pool_.Find(id);
  uniform_buffer_factory_.Create(resource);
  return id;
}

void OpenGLGraphicsResourceManager::Destroy(ResourceID id) {
  auto pool_id = GetResourcePoolIDOfResourceID(id);
  if (pool_id == g_opengl_shader_resource_pool_id) {
    auto &resource = shader_pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      shader_factory_.Destroy(resource);
    }
    shader_pool_.Destroy(id);
  } else if (pool_id == g_opengl_texture_resource_pool_id) {
    auto &resource = texture_pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      texture_factory_.Destroy(resource);
    }
    texture_pool_.Destroy(id);
  } else if (pool_id == g_opengl_mesh_resource_pool_id) {
    auto &resource = mesh_pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      mesh_factory_.Destroy(resource);
    }
    mesh_pool_.Destroy(id);
  } else if (pool_id == g_opengl_pipeline_resource_pool_id) {
    auto &resource = pipeline_pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      pipeline_factory_.Destroy(resource);
    }
    pipeline_pool_.Destroy(id);
  } else if (pool_id == g_opengl_sampler_resource_pool_id) {
    auto &resource = sampler_pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      sampler_factory_.Destroy(resource);
    }
    sampler_pool_.Destroy(id);
  } else if (pool_id == g_opengl_uniform_buffer_resource_pool_id) {
    auto &resource = uniform_buffer_pool_.Find(id);
    if (resource.status() != ResourceStatus::kInvalid) {
      uniform_buffer_factory_.Destroy(resource);
    }
    uniform_buffer_pool_.Destroy(id);
  }
  Remove(id);
}

ShaderInfo OpenGLGraphicsResourceManager::QueryInfo(ResourceID id) {
  // TODO

  return ShaderInfo();
}

}

#endif // X_OPENGL
