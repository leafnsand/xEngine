#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "graphics/Graphics.h"
#include "asset/Shader.h"
#include "object/component/Camera.h"
#include "util/MeshUtil.h"
#include "window/Window.h"
#include "io/IO.h"
#include "storage/Storage.h"

#include <stb_image.h>

#include <gtc/matrix_transform.hpp>

using namespace xEngine;

class SkyBoxSample : public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "SkyBoxSample"));
    Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
    Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("texture", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("texture").string() + Path::separator());
    IO::GetInstance().AddPlaceholder("shader", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("shader").string() + Path::separator());
    IO::GetInstance().RegisterFilesystem("storage", StorageFilesystem::Creator);

    load_camera();
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

  virtual void OnWindowMousePosition(const glm::vec2 &position) override {
    static glm::vec2 last_position;

    auto delta = position - last_position;
    last_position = position;

    if (Window::GetInstance().Get(window_id_)->IsMouseButtonPressed(MouseButtonType::kRight)) {
      delta /= 10.0f;
      camera_->Rotate(delta.x, delta.y);
    }

//    if (Window::GetInstance().Get(window_id_)->IsMouseButtonPressed(MouseButtonType::kMiddle)) {
//      delta /= 50.0f;
//      camera_->Move(delta.x, delta.y);
//    }
  }

  virtual void OnWindowMouseScroll(const glm::vec2 &offset) override {
//    camera_->set_zoom(camera_->zoom() + offset.y / 10.0f);
  }

 private:
  void load_shader() {
    IO::GetInstance().Read("shader:SkyBox.shader", [&](Location location, IOStatus status, DataPtr data) {
      if (status == IOStatus::kSuccess) {
        skybox_shader_ = Shader::Parse(window_id_, data);
        skybox_shader_->Initialize();

        load_skybox_pipeline();
      }
    });
    IO::GetInstance().Read("shader:Blinn-Phong.Texture.shader", [&](Location location, IOStatus status, DataPtr data) {
      if (status == IOStatus::kSuccess) {
        cube_shader_ = Shader::Parse(window_id_, data);
        cube_shader_->Initialize();

        load_cube_pipeline();
      }
    });
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
        if (result == 1 && width > 0 && height > 0) {
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
        }
      }
    });

    for (auto index = 0; index < static_cast<uint8>(GraphicsMaxDefine::kMaxCubeTextureFaceCount); ++index) {
      char path[32] = {0};
      sprintf(path, "texture:skybox/%d.jpg", index);
      IO::GetInstance().Read(path, [=](Location location, IOStatus status, DataPtr data) {
        if (status == IOStatus::kSuccess) {
          int components;
          stbi_set_unpremultiply_on_load(1);
          stbi_convert_iphone_png_to_rgb(1);
          auto result = stbi_info_from_memory(reinterpret_cast<const stbi_uc *>(data->buffer()),
                                              static_cast<int>(data->size()),
                                              &texture_width, &texture_height, &components);
          if (result == 1 && texture_width > 0 && texture_height > 0) {
            auto buffer = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data->buffer()),
                                                static_cast<int>(data->size()),
                                                &texture_width, &texture_height, &components, STBI_rgb);
            texture_data_[index] = Data::Create(reinterpret_cast<const char *>(buffer), static_cast<size_t>(texture_width * texture_height * 4));
            stbi_image_free(buffer);

            load_skybox_texture();
          }
        }
      });
    }
  }

  void load_skybox_texture() {
    TextureConfig config;
    config.width = texture_width;
    config.height = texture_height;
    config.type = TextureType::kTextureCube;
    config.color_format = PixelFormat::RGB8;
    for (auto index = 0; index < static_cast<uint8>(GraphicsMaxDefine::kMaxCubeTextureFaceCount); ++index) {
      if (texture_data_[index] == nullptr) return;
      config.data[index][0] = texture_data_[index];
    }
    skybox_texture_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(config);
  }

  void load_mesh() {
    box_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Cube().config());
  }

  void load_skybox_pipeline() {
    VertexLayout layout;
    layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);

    auto pipeline_config = PipelineConfig::ShaderWithLayout(skybox_shader_->resource_id(), layout);
    pipeline_config.depth_stencil_state.depth_enable = true;
    pipeline_config.depth_stencil_state.depth_write_enable = true;
    pipeline_config.depth_stencil_state.depth_compare = CompareFunction::kLessEqual;
    skybox_pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);
  }

  void load_cube_pipeline() {
    VertexLayout layout;
    layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
    layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
    layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);

    auto pipeline_config = PipelineConfig::ShaderWithLayout(cube_shader_->resource_id(), layout);
    pipeline_config.depth_stencil_state.depth_enable = true;
    pipeline_config.rasterizer_state.cull_face_enable = true;
    pipeline_config.depth_stencil_state.depth_compare = CompareFunction::kLess;
    cube_pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);
  }

  void load_camera() {
    camera_ = Camera::CreateUnique();
    camera_->set_render_target(window_id_);
    camera_->set_position(glm::vec3(3.0f, 3.0f, 10.0f));
    camera_->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    camera_->set_up_direction(glm::vec3(0.0f, 1.0f, 0.0f));
  }

  void draw() {
    if (skybox_shader_ == nullptr || cube_shader_ == nullptr) return;

    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyTarget(kInvalidResourceID, ClearState::ClearAll());
    
    renderer->ApplyPipeline(skybox_pipeline_);

    skybox_shader_->UpdateResourceData("uProjection", Data::Create(glm::value_ptr(camera_->projection_matrix()), sizeof(glm::mat4)));
    skybox_shader_->UpdateResourceData("uView", Data::Create(glm::value_ptr(glm::mat4(glm::mat3(camera_->view_matrix()))), sizeof(glm::mat4)));

    skybox_shader_->UpdateResourceSampler("uTexture", sampler_);
    skybox_shader_->UpdateResourceTexture("uTexture", skybox_texture_);

    renderer->ApplyMesh(box_);
    renderer->Draw(DrawCallState::Triangles(36));

    renderer->ApplyPipeline(cube_pipeline_);

    cube_shader_->UpdateResourceData("uProjection", Data::Create(glm::value_ptr(camera_->projection_matrix()), sizeof(glm::mat4)));
    cube_shader_->UpdateResourceData("uView", Data::Create(glm::value_ptr(camera_->view_matrix()), sizeof(glm::mat4)));
    cube_shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(glm::mat4()), sizeof(glm::mat4)));

    cube_shader_->UpdateResourceSampler("uTexture", sampler_);
    cube_shader_->UpdateResourceTexture("uTexture", texture_);

    renderer->ApplyMesh(box_);
    renderer->Draw(DrawCallState::Triangles(36));

    renderer->Render();
  }

 private:
  ShaderPtr skybox_shader_;
  ShaderPtr cube_shader_;
  CameraUniquePtr camera_;

  DataPtr texture_data_[static_cast<uint8>(GraphicsMaxDefine::kMaxCubeTextureFaceCount)];

  int32 texture_width, texture_height;

  ResourceID box_{kInvalidResourceID};

  ResourceID texture_{kInvalidResourceID};
  ResourceID skybox_texture_{kInvalidResourceID};

  ResourceID sampler_{kInvalidResourceID};

  ResourceID skybox_pipeline_{kInvalidResourceID};
  ResourceID cube_pipeline_{kInvalidResourceID};

  ResourceID window_id_{kInvalidResourceID};
};

XENGINE_WINDOW_APPLICATION(SkyBoxSample)