#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "graphics/Graphics.h"
#include "util/MeshUtil.h"
#include "asset/Shader.h"
#include "window/Window.h"
#include "io/IO.h"
#include "storage/Storage.h"

#include <stb_image.h>

#include <gtc/matrix_transform.hpp>

#include <EASTL/chrono.h>

using namespace xEngine;

class ShapeSample : public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "ShapeSample"));
    Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
    Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("texture", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("texture").string() + Path::separator());
    IO::GetInstance().AddPlaceholder("shader", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("shader").string() + Path::separator());
    IO::GetInstance().RegisterFilesystem("storage", StorageFilesystem::Creator);

    start_time_ = eastl::chrono::high_resolution_clock::now();

    load_shader();
    load_sampler();
    load_texture();
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
    IO::GetInstance().Read("shader:Blinn-Phong.Texture.hlsl", [&](Location location, IOStatus status, DataPtr data) {
      if (status == IOStatus::kSuccess) {
        shader_ = Shader::Parse(window_id_, data);
        shader_->pipeline_config().vertex_layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
        shader_->pipeline_config().vertex_layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
        shader_->pipeline_config().vertex_layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);
        shader_->pipeline_config().depth_stencil_state.depth_enable = true;
        shader_->pipeline_config().depth_stencil_state.depth_write_enable = true;
        shader_->pipeline_config().rasterizer_state.cull_face_enable = true;
        shader_->Initialize();

        auto view = glm::lookAt(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        shader_->UpdateResourceData("uView", Data::Create(glm::value_ptr(view), sizeof(view)));
        auto projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 100.0f);
        shader_->UpdateResourceData("uProjection", Data::Create(glm::value_ptr(projection), sizeof(projection)));

        load_mesh();
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
          sampler_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(SamplerConfig());
        }
      }
    });
  }

  void load_mesh() {
    cube_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Cube().config());

    sphere_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Sphere(sphere_division_).config());

    capsule_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Capsule(capsule_division_).config());

    cylinder_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Cylinder(cylinder_division_).config());

    plane_mesh_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(MeshUtil::Plane().config());
  }

  void draw() {
    if (shader_ == nullptr) return;
    
    auto now = eastl::chrono::high_resolution_clock::now();
    auto time = eastl::chrono::duration_cast<eastl::chrono::duration<float>>(now - start_time_).count();

    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->ApplyTarget(kInvalidResourceID, ClearState::ClearAll());
    
    shader_->Apply();
    
    shader_->UpdateResourceTexture("uTexture", texture_);
    shader_->UpdateResourceSampler("uSampler", sampler_);

    glm::mat4 rotation;
    rotation = glm::rotate(rotation, 0.3f * time * glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    rotation = glm::rotate(rotation, 0.6f * time * glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    rotation = glm::rotate(rotation, 0.9f * time * glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    renderer->ApplyMesh(cube_mesh_);
    const auto cube_translate = glm::translate(glm::mat4(), glm::vec3(-3.5f, 1.5f, 0.0f));
    auto cube_model = cube_translate * rotation;
    shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(cube_model), sizeof(cube_model)));
    renderer->Draw(DrawCallState::Triangles(36));

    renderer->ApplyMesh(sphere_mesh_);
    const auto sphere_translate = glm::translate(glm::mat4(), glm::vec3(0.0f, 1.5f, 0.0f));
    auto sphere_model = sphere_translate * rotation;
    shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(sphere_model), sizeof(sphere_model)));
    renderer->Draw(DrawCallState::Triangles(6 * 6 * sphere_division_ * sphere_division_));

    renderer->ApplyMesh(capsule_mesh_);
    const auto capsule_translate = glm::translate(glm::mat4(), glm::vec3(3.5f, 1.5f, 0.0f));
    auto capsule_model = capsule_translate * rotation;
    shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(capsule_model), sizeof(capsule_model)));
    renderer->Draw(DrawCallState::Triangles(6 * 6 * capsule_division_ * capsule_division_ + 4 * 6 * capsule_division_));

    renderer->ApplyMesh(cylinder_mesh_);
    const auto cylinder_translate = glm::translate(glm::mat4(), glm::vec3(-2.0f, -1.5f, 0.0f));
    auto cylinder_model = cylinder_translate * rotation;
    shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(cylinder_model), sizeof(cylinder_model)));
    renderer->Draw(DrawCallState::Triangles(12 * cylinder_division_));

    renderer->ApplyMesh(plane_mesh_);
    const auto plane_translate = glm::translate(glm::mat4(), glm::vec3(2.0f, -1.5f, 0.0f));
    auto plane_model = plane_translate * rotation;
    shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(plane_model), sizeof(plane_model)));
    renderer->Draw(DrawCallState::Triangles(6));

    renderer->Render();
  }

 private:
  size_t sphere_division_{8};
  size_t capsule_division_{8};
  size_t cylinder_division_{20};

  eastl::chrono::time_point<eastl::chrono::high_resolution_clock> start_time_;

  ShaderPtr shader_;

  ResourceID texture_{kInvalidResourceID};
  ResourceID sampler_{kInvalidResourceID};

  ResourceID cube_mesh_{kInvalidResourceID};
  ResourceID sphere_mesh_{kInvalidResourceID};
  ResourceID capsule_mesh_{kInvalidResourceID};
  ResourceID cylinder_mesh_{kInvalidResourceID};
  ResourceID plane_mesh_{kInvalidResourceID};

  ResourceID window_id_{kInvalidResourceID};
};

XENGINE_WINDOW_APPLICATION(ShapeSample)