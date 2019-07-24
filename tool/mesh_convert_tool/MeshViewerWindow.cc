#include "MeshViewerWindow.h"

#include <io/IO.h>
#include <window/Window.h>

#include <stb_image.h>
#include <gtc/matrix_transform.hpp>

static const char *texture_name[] = {
    "texture:lina_arms.tga",
    "texture:lina_base.tga",
    "texture:lina_belt.tga",
    "texture:lina_head.tga",
    "texture:lina_neck.tga",
};

namespace xEngine {

static const auto window_width = 800;

static const auto window_height = 600;

MeshViewerWindow::MeshViewerWindow() {
  x_assert(Window::GetInstance().Available());
  window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, window_width, window_height, "MeshViewer"));
  Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
  Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

  InitializeShader();
  InitializeCamera();
  InitializeTexture();
}

MeshViewerWindow::~MeshViewerWindow() {
  if (window_id_ != kInvalidResourceID) {
    MeshViewerWindow::OnWindowClose();
  }
}

void MeshViewerWindow::OnWindowUpdate() {
  if (shader_ == nullptr) return;

  auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();

  renderer->MakeCurrent();

  renderer->ApplyTarget(kInvalidResourceID, ClearState::ClearAll());
  renderer->ApplyPipeline(pipeline_);

  shader_->UpdateResourceData("uProjection", Data::Create(glm::value_ptr(camera_->projection_matrix()), sizeof(glm::mat4)));
  shader_->UpdateResourceData("uView", Data::Create(glm::value_ptr(camera_->view_matrix()), sizeof(glm::mat4)));

  shader_->UpdateResourceSampler("uTexture", sampler_);

  for (auto tuple : mesh_) {
    auto id = eastl::get<0>(tuple);
    auto texture = eastl::get<1>(tuple);
    auto state = eastl::get<2>(tuple);
    shader_->UpdateResourceTexture("uTexture", texture);
    renderer->ApplyMesh(id);
    renderer->Draw(state);
  }

  renderer->Render();
}

void MeshViewerWindow::OnWindowMousePosition(const glm::vec2 &position) {
  static glm::vec2 last_position;

  auto delta = position - last_position;
  last_position = position;

  if (Window::GetInstance().Get(window_id_)->IsMouseButtonPressed(MouseButtonType::kRight)) {
    delta /= 10.0f;
    camera_->Rotate(delta.x, delta.y);
  }

  if (Window::GetInstance().Get(window_id_)->IsMouseButtonPressed(MouseButtonType::kMiddle)) {
    delta /= 50.0f;
    camera_->Move(delta.x, delta.y);
  }
}

void MeshViewerWindow::OnWindowMouseScroll(const glm::vec2 &offset) {
  camera_->set_zoom(camera_->zoom() + offset.y / 10.0f);
}

void MeshViewerWindow::OnWindowClose() {
  Window::GetInstance().GetGraphics(window_id_)->Finalize();
  Window::GetInstance().Destroy(window_id_);
  window_id_ = kInvalidResourceID;
}

void MeshViewerWindow::Show(SceneInfo &scene) {
  mesh_.clear();
  for (auto &node_info : scene.root.children) {
    for (auto mesh_index : node_info.meshes) {
      auto &mesh_info = scene.meshes[mesh_index];
      eastl::string name = "texture:";
      ResourceID texture_id = kInvalidResourceID;
      if (node_info.name == "lina_arms") {
        name += "lina_arms.tga";
        auto pair = textures_.find(name);
        if (pair != textures_.end()) {
          texture_id = pair->second;
        }
      } else if (node_info.name == "lina_base") {
        name += "lina_base.tga";
        auto pair = textures_.find(name);
        if (pair != textures_.end()) {
          texture_id = pair->second;
        }
      } else if (node_info.name == "lina_belt") {
        name += "lina_belt.tga";
        auto pair = textures_.find(name);
        if (pair != textures_.end()) {
          texture_id = pair->second;
        }
      } else if (node_info.name == "lina_belt_dress") {
        name += "lina_belt.tga";
        auto pair = textures_.find(name);
        if (pair != textures_.end()) {
          texture_id = pair->second;
        }
      } else if (node_info.name == "lina_head") {
        name += "lina_head.tga";
        auto pair = textures_.find(name);
        if (pair != textures_.end()) {
          texture_id = pair->second;
        }
      } else if (node_info.name == "lina_neck") {
        name += "lina_neck.tga";
        auto pair = textures_.find(name);
        if (pair != textures_.end()) {
          texture_id = pair->second;
        }
      }
      Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();
      auto id = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(mesh_info.util.config());
      auto draw_call_state = DrawCallState::Triangles(mesh_info.util.config().index_count);
      mesh_.push_back(eastl::make_tuple(id, texture_id, draw_call_state));
    }
  }
}

void MeshViewerWindow::InitializeShader() {
  IO::GetInstance().Read("shader:Blinn-Phong.Texture.shader", [&](Location location, IOStatus status, DataPtr data) {
    if (status == IOStatus::kSuccess) {
      Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

      shader_ = Shader::Parse(window_id_, data);
      shader_->Initialize();
      shader_->Apply();
      shader_->UpdateResourceData("uModel", Data::Create(glm::value_ptr(glm::mat4()), sizeof(glm::mat4)));

      InitializePipeline();
    }
  });
}

void MeshViewerWindow::InitializeCamera() {
  camera_ = Camera::CreateUnique();
  camera_->set_render_window(window_id_);
  camera_->set_position(glm::vec3(0.0f, 0.0f, 400.0f));
  camera_->set_target(glm::vec3(0.0f, 100.0f, 0.0f));
  camera_->set_up_direction(glm::vec3(0.0f, 1.0f, 0.0f));
}

void MeshViewerWindow::InitializePipeline() {
  VertexLayout layout;
  layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);
  layout.AddElement(VertexElementSemantic::kTexcoord0, VertexElementFormat::kFloat2);
  layout.AddElement(VertexElementSemantic::kNormal, VertexElementFormat::kFloat3);

  auto pipeline_config = PipelineConfig::ShaderWithLayout(shader_->resource_id(), layout);
  pipeline_config.depth_stencil_state.depth_enable = true;
  pipeline_config.depth_stencil_state.depth_write_enable = true;
  pipeline_config.rasterizer_state.cull_face_enable = true;
  pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);
}

void MeshViewerWindow::InitializeTexture() {
  sampler_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(SamplerConfig());

  for (auto index = 0; index < EAArrayCount(texture_name); ++index) {
    auto name = texture_name[index];
    IO::GetInstance().Read(name, [this, name](Location location, IOStatus status, DataPtr data) {
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
          Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();
          auto id = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(config);
          textures_.insert(eastl::make_pair(name, id));
        }
      }
    });
  }
}

}
