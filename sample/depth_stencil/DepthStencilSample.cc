#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "graphics/Graphics.h"
#include "util/MeshUtil.h"
#include "window/Window.h"
#include "io/IO.h"
#include "storage/Storage.h"

#include <stb_image.h>

#include <gtc/matrix_transform.hpp>

#include <EASTL/chrono.h>

static const char *shader_source =
  "float4x4 uModel;\n"
  "float4x4 uView;\n"
  "float4x4 uProjection;\n"
  "struct VS_INPUT\n"
  "{\n"
  "    float3 aPosition : POSITION;\n"
  "    float2 aTexcoord0 : TEXCOORD;\n"
  "};\n"
  "struct VS_OUTPUT\n"
  "{\n"
  "    float4 Position : SV_Position;\n"
  "    float2 Texcoord : TEXCOORD;\n"
  "};\n"
  "VS_OUTPUT VS(const VS_INPUT input)\n"
  "{\n"
  "    VS_OUTPUT output;\n"
  "    output.Texcoord = input.aTexcoord0;\n"
  "    output.Position = mul(uProjection, mul(uView, mul(uModel, float4(input.aPosition, 1.0))));\n"
  "    return output;\n"
  "}\n"
  "#define PS_INPUT VS_OUTPUT\n"
  "struct PS_OUTPUT\n"
  "{\n"
  "    float4 Color: SV_Target;\n"
  "};\n"
  "float3 uColor;\n"
  "Texture2D uTexture;\n"
  "SamplerState uSampler;\n"
  "PS_OUTPUT PS(const PS_INPUT input)\n"
  "{\n"
  "    PS_OUTPUT output;\n"
  "    output.Color = uTexture.Sample(uSampler, input.Texcoord) * float4(uColor, 1.0);\n"
  "    return output;\n"
  "}\n";

using namespace xEngine;

class DepthStencilSample : public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "DepthStencilSample"));
    Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
    Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("texture", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("texture").string() + Path::separator());
    IO::GetInstance().RegisterFilesystem("storage", StorageFilesystem::Creator);

    start_time_ = eastl::chrono::high_resolution_clock::now();

    load_shader();
    load_sampler();
    load_texture();
    load_mesh();
  }

  virtual void Finalize() override {
    IO::GetInstance().Finalize();
    Window::GetInstance().Finalize();
  }

  virtual void OnWindowClose() override {
    Window::GetInstance().GetGraphics(window_id_)->Finalize();
    Window::GetInstance().Destroy(window_id_);
    window_id_ = kInvalidResourceID;
#if X_WINDOWS
    Application::GetInstance().Quit();
#endif
  }

  virtual void OnWindowUpdate() override {
    draw();
  }

 private:
  void load_shader() {
    auto shader_config = ShaderConfig::FromSource(shader_source);
    shader_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(shader_config);
    Window::GetInstance().GetGraphics(window_id_)->renderer()->ApplyShader(shader_);
    auto view = glm::lookAt(
        glm::vec3(5.0f, 5.0f, 4.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    Window::GetInstance().GetGraphics(window_id_)->renderer()->UpdateShaderResourceData(shader_, "uView", Data::Create(glm::value_ptr(view), sizeof(view)));
    auto projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 10.0f);
    Window::GetInstance().GetGraphics(window_id_)->renderer()->UpdateShaderResourceData(shader_, "uProjection", Data::Create(glm::value_ptr(projection), sizeof(projection)));
  }

  void load_sampler() {
    SamplerConfig sampler_config;
    sampler_config.wrap_mod_s = TextureWrapMode::kClampToEdge;
    sampler_config.wrap_mod_t = TextureWrapMode::kClampToEdge;
    sampler_config.wrap_mod_r = TextureWrapMode::kClampToEdge;
    sampler_config.filter_mode_min = TextureFilterMode::kLinear;
    sampler_config.filter_mode_mag = TextureFilterMode::kLinear;
    sampler_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(sampler_config);
  }

  void load_texture() {
    IO::GetInstance().Read("texture:The_Var_department.jpg", [&](Location location, IOStatus status, DataPtr data) {
      if (status == IOStatus::kSuccess) {
        int width, height, components;
        stbi_set_unpremultiply_on_load(1);
        stbi_convert_iphone_png_to_rgb(1);
        auto result = stbi_info_from_memory(reinterpret_cast<const stbi_uc *>(data->buffer()),
                                            static_cast<int>(data->size()),
                                            &width, &height, &components);
        if (result == 1 && width > 0 && height > 0)
        {
          auto buffer = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data->buffer()),
                                              static_cast<int>(data->size()),
                                              &width, &height, &components, STBI_rgb_alpha);
          auto pixel_data = Data::Create(reinterpret_cast<const char *>(buffer), static_cast<size_t>(width * height * 4));
          stbi_image_free(buffer);
          TextureConfig config;
          config.width = width;
          config.height = height;
          config.color_format = PixelFormat::RGBA8;
          config.data[0][0] = pixel_data;
          texture_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(config);
          sampler_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(SamplerConfig());
        }
      }
    });
  }

  void load_mesh() {
    VertexLayout layout;
    layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3)
          .AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);

    cube_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Cube().config());

    plane_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Plane().config());

    auto pipeline_config = PipelineConfig::ShaderWithLayout(shader_, layout);
    pipeline_config.depth_stencil_state.depth_enable = true;
    pipeline_config.depth_stencil_state.depth_write_enable = true;
    cube_pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);

    pipeline_config.depth_stencil_state.depth_enable = true;
    pipeline_config.depth_stencil_state.depth_write_enable = false;
    pipeline_config.depth_stencil_state.stencil_enable = true;
    pipeline_config.depth_stencil_state.stencil_read_mask = 0xff;
    pipeline_config.depth_stencil_state.stencil_write_mask = 0xff;
    pipeline_config.depth_stencil_state.stencil_value = 1;
    pipeline_config.depth_stencil_state.front.compare = CompareFunction::kAlways;
    pipeline_config.depth_stencil_state.front.fail = StencilOperation::kKeep;
    pipeline_config.depth_stencil_state.front.depth_fail = StencilOperation::kKeep;
    pipeline_config.depth_stencil_state.front.pass = StencilOperation::kReplace;
    pipeline_config.depth_stencil_state.back = pipeline_config.depth_stencil_state.front;
    floor_pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);

    pipeline_config.depth_stencil_state.depth_enable = true;
    pipeline_config.depth_stencil_state.depth_write_enable = true;
    pipeline_config.depth_stencil_state.stencil_enable = true;
    pipeline_config.depth_stencil_state.stencil_read_mask = 0xff;
    pipeline_config.depth_stencil_state.stencil_write_mask = 0x00;
    pipeline_config.depth_stencil_state.stencil_value = 1;
    pipeline_config.depth_stencil_state.front.compare = CompareFunction::kEqual;
    pipeline_config.depth_stencil_state.front.fail = StencilOperation::kKeep;
    pipeline_config.depth_stencil_state.front.depth_fail = StencilOperation::kKeep;
    pipeline_config.depth_stencil_state.front.pass = StencilOperation::kReplace;
    pipeline_config.depth_stencil_state.back = pipeline_config.depth_stencil_state.front;
    reflection_pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);
  }

  void draw() {
    auto now = eastl::chrono::high_resolution_clock::now();
    auto time = eastl::chrono::duration_cast<eastl::chrono::duration<float>>(now - start_time_).count();

    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyTarget(kInvalidResourceID, ClearState::ClearAll());

    draw_cube(time);
    draw_plane(time);
    draw_reflection(time);

    renderer->Render();
  }

  void draw_cube(float32 time) {
    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyMesh(cube_mesh_);
    renderer->ApplyPipeline(cube_pipeline_);
    auto cube_model = glm::rotate(
        glm::translate(glm::mat4(), glm::vec3(0.0f, 1.0f, 0.0f)),
        time * glm::radians(180.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    renderer->UpdateShaderResourceData(shader_, "uModel", Data::Create(glm::value_ptr(cube_model), sizeof(cube_model)));
    renderer->UpdateShaderResourceData(shader_, "uColor", Data::Create(glm::value_ptr(Color(1.0, 1.0, 1.0, 1.0)), sizeof(Color)));
    renderer->UpdateShaderResourceTexture(shader_, "uTexture", texture_);
    renderer->UpdateShaderResourceSampler(shader_, "uSampler", sampler_);
    renderer->Draw(DrawCallState::Triangles(36));
  }

  void draw_plane(float32 time) {
    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyMesh(plane_mesh_);
    renderer->ApplyPipeline(floor_pipeline_);
    auto plane_model = glm::scale(
        glm::mat4(),
        glm::vec3(4.0f, 4.0f, 4.0f)
    );
    renderer->UpdateShaderResourceData(shader_, "uModel", Data::Create(glm::value_ptr(plane_model), sizeof(plane_model)));
    renderer->UpdateShaderResourceData(shader_, "uColor", Data::Create(glm::value_ptr(Color(0.0, 0.0, 0.0, 1.0)), sizeof(Color)));
    renderer->UpdateShaderResourceTexture(shader_, "uTexture", texture_);
    renderer->UpdateShaderResourceSampler(shader_, "uSampler", sampler_);
    renderer->Draw(DrawCallState::Triangles(6));
  }

  void draw_reflection(float32 time) {
    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyMesh(cube_mesh_);
    renderer->ApplyPipeline(reflection_pipeline_);
    auto reflection_model = glm::scale(
        glm::rotate(
            glm::translate(glm::mat4(), glm::vec3(0.0f, -1.0f, 0.0f)),
            time * glm::radians(180.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        ),
        glm::vec3(1, -1, 1)
    );
    renderer->UpdateShaderResourceData(shader_, "uModel",
                                       Data::Create(glm::value_ptr(reflection_model), sizeof(reflection_model)));
    renderer->UpdateShaderResourceData(shader_, "uColor", Data::Create(glm::value_ptr(Color(0.3, 0.3, 0.3, 1.0)), sizeof(Color)));
    renderer->UpdateShaderResourceTexture(shader_, "uTexture", texture_);
    renderer->UpdateShaderResourceSampler(shader_, "uSampler", sampler_);
    renderer->Draw(DrawCallState::Triangles(36));
  }

 private:
  eastl::chrono::time_point<eastl::chrono::high_resolution_clock> start_time_;
  ResourceID shader_{kInvalidResourceID};
  ResourceID texture_{kInvalidResourceID};
  ResourceID sampler_{kInvalidResourceID};
  ResourceID cube_mesh_{kInvalidResourceID};
  ResourceID plane_mesh_{kInvalidResourceID};
  ResourceID cube_pipeline_{kInvalidResourceID};
  ResourceID floor_pipeline_{kInvalidResourceID};
  ResourceID reflection_pipeline_{kInvalidResourceID};
  ResourceID window_id_{kInvalidResourceID};
};

XENGINE_WINDOW_APPLICATION(DepthStencilSample)