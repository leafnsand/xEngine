#include "application/ApplicationDelegate.h"
#include "core/Log.h"
#include "graphics/Graphics.h"
#include "window/Window.h"
#include "io/IO.h"
#include "storage/Storage.h"
#include "asset/graphics/MeshLoader.h"
#include "asset/graphics/Camera.h"

#include <gtc/matrix_transform.hpp>

#include <EASTL/chrono.h>
#include <EASTL/tuple.h>

#if X_OPENGL

static const char *vertex_shader =
    "#version 410 core\n"
    "in vec3 aPosition;\n"
    "uniform mat4 uModel;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProjection;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);\n"
    "}\n";

static const char *fragment_shader =
    "#version 410 core\n"
    "out vec4 outColor;\n"
    "void main()\n"
    "{\n"
    "    outColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
    "}\n";

#elif X_D3D11

static const char *vertex_shader =
  "float4x4 uModel;\n"
  "float4x4 uView;\n"
  "float4x4 uProjection;\n"
  "struct VS_INPUT\n"
  "{\n"
  "    float3 aPosition: POSITION;\n"
  "};\n"
  "struct VS_OUTPUT\n"
  "{\n"
  "    float4 Position: SV_POSITION;\n"
  "};\n"
  "VS_OUTPUT main(const VS_INPUT input)\n"
  "{\n"
  "    VS_OUTPUT output;\n"
  "    output.Position = mul(uProjection, mul(uView, mul(uModel, float4(input.aPosition, 1.0))));\n"
  "    return output;\n"
  "}\n";

static const char *fragment_shader =
  "struct PS_INPUT\n"
  "{\n"
  "    float4 Position: SV_POSITION;\n"
  "};\n"
  "float4 main(const PS_INPUT input): SV_TARGET\n"
  "{\n"
  "    return float4(1.0, 0.0, 0.0, 1.0);\n"
  "}\n";

#endif

using namespace xEngine;

class MeshSample : public ApplicationDelegate, WindowDelegate {
 public:
  virtual void Initialize() override {
    Window::GetInstance().Initialize();
    window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, 1024, 768, "MeshSample"));
    Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
    Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();

    IO::GetInstance().Initialize();
    IO::GetInstance().AddPlaceholder("texture", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("texture").string() + Path::separator());
    IO::GetInstance().AddPlaceholder("mesh", "storage://" +
        Path::ExecutablePath().ParentDirectory().ParentDirectory().Append("assets").Append("mesh").string() + Path::separator());
    IO::GetInstance().RegisterFilesystem("storage", StorageFilesystem::Creator);

    load_shader();
    load_camera();
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

    if (Window::GetInstance().Get(window_id_)->IsMouseButtonPressed(MouseButtonType::kMiddle)) {
      delta /= 50.0f;
      camera_->Move(delta.x, delta.y);
    }
  }

  virtual void OnWindowMouseScroll(const glm::vec2 &offset) override {
    camera_->set_zoom(camera_->zoom() + offset.y / 10.0f);
  }

 private:
  void load_shader() {
    auto shader_config = ShaderConfig::FromString(vertex_shader, fragment_shader);
    shader_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(shader_config);
  }

  void load_camera() {
    camera_ = Camera::CreateUnique();
    camera_->set_render_window(window_id_);
    camera_->set_position(glm::vec3(0.0f, 0.0f, 3.0f));
    camera_->set_target(glm::vec3(0.0f, 0.0f, 0.0f));
    camera_->set_up_direction(glm::vec3(0.0f, 1.0f, 0.0f));

    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
    renderer->UpdateShaderResourceData(shader_, "uModel", Data::Create(glm::value_ptr(glm::mat4()), sizeof(glm::mat4)));
  }

  void load_mesh() {
    IO::GetInstance().Read("mesh:teapot.xmesh", [&](Location location, IOStatus status, DataPtr data) {
      if (status == IOStatus::kSuccess) {
        auto mesh_vector = MeshLoader::Load(data);
        for (auto mesh : mesh_vector) {
          auto id = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(mesh.config());
          auto state = DrawCallState::Triangles(mesh.config().index_count);
          mesh_.push_back(eastl::make_tuple(id, state));
        }
      }
    });

    VertexLayout layout;
    layout.AddElement(VertexElementSemantic::kPosition, VertexElementFormat::kFloat3);

    auto pipeline_config = PipelineConfig::ShaderWithLayout(shader_, layout);
    pipeline_config.depth_stencil_state.depth_enable = true;
    pipeline_config.depth_stencil_state.depth_write_enable = true;
    pipeline_config.rasterizer_state.fill_mode = FillMode::kWireframe;
    pipeline_config.rasterizer_state.cull_face_enable = true;
    pipeline_ = Window::GetInstance().GetGraphics(window_id_)->resource_manager()->Create(pipeline_config);
  }

  void draw() {
    auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();

    renderer->ApplyTarget(kInvalidResourceID, ClearState::ClearAll());
    renderer->ApplyShader(shader_);
    renderer->ApplyPipeline(pipeline_);

    renderer->UpdateShaderResourceData(shader_, "uProjection", Data::Create(glm::value_ptr(camera_->projection_matrix()), sizeof(glm::mat4)));
    renderer->UpdateShaderResourceData(shader_, "uView", Data::Create(glm::value_ptr(camera_->view_matrix()), sizeof(glm::mat4)));

    for (auto tuple : mesh_) {
      auto id = eastl::get<0>(tuple);
      auto state = eastl::get<1>(tuple);
      renderer->ApplyMesh(id);
      renderer->Draw(state);
    }

    renderer->Render();
  }

 private:
  CameraUniquePtr camera_;
  ResourceID shader_{kInvalidResourceID};
  ResourceID pipeline_{kInvalidResourceID};
  ResourceID window_id_{kInvalidResourceID};
  eastl::vector<eastl::tuple<ResourceID, DrawCallState>> mesh_;
};

XENGINE_WINDOW_APPLICATION(MeshSample)