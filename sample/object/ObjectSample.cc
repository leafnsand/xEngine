#include "application/ApplicationDelegate.h"
#include "window/Window.h"
#include "io/IO.h"
#include "storage/Storage.h"
#include "object/core/Scene.h"
#include "object/component/Camera.h"
#include "object/component/MeshRenderer.h"
#include "asset/Shader.h"
#include "asset/Mesh.h"
#include "util/MeshUtil.h"

#include <stb_image.h>

using namespace xEngine;

class ObjectSample : public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "ObjectSample"));
    Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
    Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("texture", "storage://" +
      Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("texture").string() + Path::separator());
    IO::GetInstance().AddPlaceholder("shader", "storage://" +
      Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("shader").string() + Path::separator());
    IO::GetInstance().RegisterFilesystem("storage", StorageFilesystem::Creator);

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

          texture_ = Texture::Create();
          texture_->set_texture_id(Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(config));
          texture_->set_sampler_id(Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(SamplerConfig()));

          if (cube_id) {
            auto cube = Scene::GetInstance().FindGameObject(cube_id);
            auto cube_component = cube->GetComponent<MeshRenderer>(ComponentType::kMeshRenderer);
            cube_component->GetMaterial(0)->BindTexture("uTexture", texture_);
          }
        }
      }
    });

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

        auto cube = Scene::GetInstance().AddGameObject();
        auto cube_component = eastl::static_pointer_cast<MeshRenderer>(cube->AttachComponent(ComponentType::kMeshRenderer));
        cube_component->set_mesh(cube_mesh_);
        cube_component->SetMaterialCount(1);
        auto &material = cube_component->GetMaterial(0);
        material.reset(new Material);
        material->set_shader(shader_);

        if (texture_) material->BindTexture("uTexture", texture_);
      }
    });

    cube_mesh_ = Mesh::Parse(window_id_, MeshUtil::Cube());
    cube_mesh_->Initialize();

    Scene::GetInstance().Initialize();

    auto camera = Scene::GetInstance().AddGameObject();
    auto camera_component = eastl::static_pointer_cast<Camera>(camera->AttachComponent(ComponentType::kCamera));
    camera_component->set_render_target(window_id_);
    camera_component->set_clear_type(CameraClearType::kSolidColor);
    camera_component->set_position(glm::vec3(0.0f, 0.0f, 3.0f));
    camera_component->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    camera_component->set_up_direction(glm::vec3(0.0f, 1.0f, 0.0f));

    Camera::set_main_camera(camera_component);
  }

  virtual void Finalize() override {
    IO::GetInstance().Finalize();
    Window::GetInstance().Finalize();
  }

  virtual void OnWindowClose() override {
    if (shader_) shader_->Finalize();
    cube_mesh_->Finalize();

    Scene::GetInstance().Finalize();

    Window::GetInstance().Destroy(window_id_);
    window_id_ = kInvalidResourceID;
#if X_WINDOWS
    Application::GetInstance().Quit();
#endif
  }

  virtual void OnWindowUpdate() override {
    Scene::GetInstance().Update();
  }

 private:
  ResourceID window_id_{kInvalidResourceID};
  MeshPtr cube_mesh_;
  ShaderPtr shader_;
  TexturePtr texture_;
  uint32 cube_id{0};
};

XENGINE_WINDOW_APPLICATION(ObjectSample)