#if X_D3D11

#include "D3D11ShaderFactory.h"

#include "graphics/impl/D3D11/D3D11Define.h"

#include "core/Log.h"

#include <EASTL/allocator.h>

namespace xEngine {

static DataPtr CompileShader(const char *type, const char *source, const char *entry_point) {
  DataPtr result = nullptr;

  ID3D10Blob *shader = nullptr;
  ID3D10Blob *log = nullptr;

  auto flag = D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_WARNINGS_ARE_ERRORS;
#if X_DEBUG
  flag |= D3D10_SHADER_DEBUG;
#else
    vertex_flag |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

  if (FAILED(D3DCompile(
    source,
    strlen(source),
    nullptr,
    nullptr,
    nullptr,
    entry_point,
    type,
    flag,
    0,
    &shader,
    &log
  ))) {
    if (log != nullptr) {
      Log::GetInstance().Error("compile error: \n%s\n%s\n", source, log->GetBufferPointer());
      log->Release();
      log = nullptr;
    }
  }
  if (shader != nullptr) {
    result = Data::Create(shader->GetBufferPointer(), shader->GetBufferSize());
    shader->Release();
    shader = nullptr;
  }

  return result;
}

static void ReflectShader(ID3D11Device *device, DataPtr blob, void **data, ID3D11Buffer **buffer,
                          VertexLayout *input_layout,
                          ShaderInfo::UniformBlockInfo &global_info,
                          eastl::hash_map<eastl::string, ShaderInfo::UniformBlockInfo> &block_map,
                          eastl::hash_map<eastl::string, uint32> &texture_map,
                          eastl::hash_map<eastl::string, uint32> &sampler_map) {
  ID3D11ShaderReflection *reflection = nullptr;

  x_d3d11_assert_msg(D3DReflect(
    blob->buffer(),
    blob->size(),
    IID_ID3D11ShaderReflection,
    reinterpret_cast<void **>(&reflection)
  ), "create reflect failed\n");

  D3D11_SHADER_DESC desc;
  x_d3d11_assert_msg(reflection->GetDesc(&desc), "get description failed\n");

  auto uniform_count = 0;
  auto texture_count = 0;

  if (input_layout) {
    for (auto input_index = 0; input_index < desc.InputParameters; ++input_index) {
      D3D11_SIGNATURE_PARAMETER_DESC input_desc;
      ZeroMemory(&input_desc, sizeof(input_desc));
      x_d3d11_assert_msg(reflection->GetInputParameterDesc(input_index, &input_desc), "get input description failed\n");

      input_layout->AddElement(VertexElementSemanticForSemanticAndIndex(input_desc.SemanticName, input_desc.SemanticIndex),
                               VertexElementFormatForComponentTypeAndMask(input_desc.ComponentType, input_desc.Mask));
    }
  }

  for (auto resource_index = 0; resource_index < desc.BoundResources; ++resource_index) {
    D3D11_SHADER_INPUT_BIND_DESC resource_desc;
    ZeroMemory(&resource_desc, sizeof(resource_desc));
    x_d3d11_assert_msg(reflection->GetResourceBindingDesc(resource_index, &resource_desc), "get resource description failed\n");

    if (resource_desc.Type == D3D_SIT_CBUFFER) {
      ++uniform_count;
      x_assert(uniform_count < static_cast<uint16>(GraphicsMaxDefine::kMaxUniformBlockCount));

      ShaderInfo::UniformBlockInfo block_info;
      block_info.location = resource_desc.BindPoint;

      auto uniform_block_reflection = reflection->GetConstantBufferByName(resource_desc.Name);

      D3D11_SHADER_BUFFER_DESC uniform_block_desc;
      ZeroMemory(&uniform_block_desc, sizeof(uniform_block_desc));
      x_d3d11_assert_msg(uniform_block_reflection->GetDesc(&uniform_block_desc), "get constant reflection failed\n");
      block_info.size = uniform_block_desc.Size;

      for (auto variable_index = 0; variable_index < uniform_block_desc.Variables; ++variable_index) {
        auto variable_reflection = uniform_block_reflection->GetVariableByIndex(variable_index);

        D3D11_SHADER_VARIABLE_DESC variable_desc;
        ZeroMemory(&variable_desc, sizeof(variable_desc));
        x_d3d11_assert_msg(variable_reflection->GetDesc(&variable_desc), "get variable description failed\n");
        auto variable_type_reflection = variable_reflection->GetType();

        D3D11_SHADER_TYPE_DESC variable_type_desc;
        ZeroMemory(&variable_type_desc, sizeof(variable_type_desc));
        x_d3d11_assert_msg(variable_type_reflection->GetDesc(&variable_type_desc), "get variable type description failed\n");

        ShaderInfo::UniformElementInfo element_info;
        element_info.offset = variable_desc.StartOffset;
        element_info.size = variable_desc.Size;

        block_info.elements.insert({ variable_desc.Name, element_info });
      }

      if (strcmp(uniform_block_desc.Name, "$Globals") == 0 && uniform_block_desc.Size > 0) {
        ID3D11Buffer *uniform_buffer = nullptr;

        D3D11_BUFFER_DESC uniform_buffer_desc;
        ZeroMemory(&uniform_buffer_desc, sizeof(uniform_buffer_desc));

        uniform_buffer_desc.ByteWidth = uniform_block_desc.Size;
        uniform_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
        uniform_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        uniform_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        x_d3d11_assert_msg(device->CreateBuffer(&uniform_buffer_desc, nullptr, &uniform_buffer), "create global uniform buffer failed\n");

        *data = eastl::GetDefaultAllocator()->allocate(block_info.size);
        *buffer = uniform_buffer;
        global_info = block_info;
      } else {
        block_map.insert({ uniform_block_desc.Name, block_info });
      }
    } else if (resource_desc.Type == D3D_SIT_TEXTURE) {
      ++texture_count;
      x_assert(uniform_count < static_cast<uint16>(GraphicsMaxDefine::kMaxTextureCount));
      texture_map.insert(eastl::make_pair(resource_desc.Name, resource_desc.BindPoint));
    } else if (resource_desc.Type == D3D_SIT_SAMPLER) {
      sampler_map.insert(eastl::make_pair(resource_desc.Name, resource_desc.BindPoint));
    }
  }

  reflection->Release();
}

void D3D11ShaderFactory::Create(D3D11Shader &resource) {
  x_assert(resource.status() == ResourceStatus::kPending);
  resource.Loading();

  auto &config = resource.config();

  ID3D11VertexShader *vertex_shader = nullptr;
  ID3D11PixelShader *fragment_shader = nullptr;

  if (config.vertex == nullptr) {
    config.vertex = CompileShader("vs_5_0", config.source.c_str(), "VS");
  }

  if (config.vertex != nullptr && config.vertex->buffer() != nullptr && config.vertex->size() > 0) {
    x_d3d11_assert_msg(device_->CreateVertexShader(
      config.vertex->buffer(),
      config.vertex->size(),
      nullptr,
      &vertex_shader
    ), "create vertex shader failed\n");

    ReflectShader(device_, config.vertex,
      &resource.vertex_global_uniform_buffer,
      &resource.vertex_global_uniform_block,
      &resource.info.input_layout,
      resource.info.vertex.global_uniform_block_info,
      resource.info.vertex.uniform_block_info,
      resource.info.vertex.texture_index,
      resource.info.vertex.sampler_index);
  }

  if (config.fragment == nullptr) {
    config.fragment = CompileShader("ps_5_0", config.source.c_str(), "PS");
  }

  if (config.fragment != nullptr && config.fragment->buffer() != nullptr && config.fragment->size() > 0) {
    x_d3d11_assert_msg(device_->CreatePixelShader(
      config.fragment->buffer(),
      config.fragment->size(),
      nullptr,
      &fragment_shader
    ), "create fragment shader failed\n");

    ReflectShader(device_, config.fragment,
      &resource.fragment_global_uniform_buffer,
      &resource.fragment_global_uniform_block,
      nullptr,
      resource.info.fragment.global_uniform_block_info,
      resource.info.fragment.uniform_block_info,
      resource.info.fragment.texture_index,
      resource.info.fragment.sampler_index);
  }

  x_assert_msg(vertex_shader, "create vertex shader failed!\n");
  x_assert_msg(fragment_shader, "create fragment shader failed!\n");

  resource.vertex_shader = vertex_shader;
  resource.fragment_shader = fragment_shader;

  resource.Complete();
}

void D3D11ShaderFactory::Destroy(D3D11Shader &resource) {
  x_assert(resource.status() == ResourceStatus::kFailed || resource.status() == ResourceStatus::kCompleted);
  if (resource.vertex_shader != nullptr) {
    resource.vertex_shader->Release();
  }
  if (resource.fragment_shader != nullptr) {
    resource.fragment_shader->Release();
  }
  if (resource.vertex_global_uniform_buffer != nullptr) {
    eastl::GetDefaultAllocator()->deallocate(resource.vertex_global_uniform_buffer, resource.info.vertex.global_uniform_block_info.size);
  }
  if (resource.vertex_global_uniform_block != nullptr) {
    resource.vertex_global_uniform_block->Release();
  }
  if (resource.fragment_global_uniform_buffer != nullptr) {
    eastl::GetDefaultAllocator()->deallocate(resource.fragment_global_uniform_buffer, resource.info.fragment.global_uniform_block_info.size);
  }
  if (resource.fragment_global_uniform_block != nullptr) {
    resource.fragment_global_uniform_block->Release();
  }
}

} // namespace xEngine

#endif // X_D3D11
