#include "MeshConvertToolWindow.h"
#include "MeshConverter.h"

#include <application/Application.h>
#include <window/Window.h>

namespace xEngine {

static const auto window_width = 800;

static const auto window_height = 600;

MeshConvertToolWindow::MeshConvertToolWindow() {
  x_assert(Window::GetInstance().Available());
  nuklear_ = NuklearGUI::CreateUnique();
  window_id_ = Window::GetInstance().Create(WindowConfig::ForWindow(this, window_width, window_height, "MeshConvertTool"));
  Window::GetInstance().GetGraphics(window_id_)->Initialize(GraphicsConfig::ForWindow(window_id_));
  Window::GetInstance().GetGraphics(window_id_)->renderer()->MakeCurrent();
  nuklear_->Initialize(NuklearGUIConfig::ForWindow(window_id_));
}

MeshConvertToolWindow::~MeshConvertToolWindow() {
  if (window_id_ != kInvalidResourceID) {
    MeshConvertToolWindow::OnWindowClose();
  }
}

void MeshConvertToolWindow::OnWindowUpdate() {
  Draw();
}

void MeshConvertToolWindow::OnWindowClose() {
  nuklear_->Finalize();
  Window::GetInstance().GetGraphics(window_id_)->Finalize();
  Window::GetInstance().Destroy(window_id_);
  window_id_ = kInvalidResourceID;
#if X_WINDOWS
  Application::GetInstance().Quit();
#endif
}

void MeshConvertToolWindow::OnWindowDropFile(eastl::vector<eastl::string> files) {
  for (auto file : files) {
    auto out_file = file;
    auto position = out_file.find_last_of('.');
    if (position == eastl::string::npos) {
      Log::GetInstance().Error("cannot resolve file extension name of %s\n", file.c_str());
      continue;
    }
    out_file.replace(position, out_file.length() - position, ".xmesh");
    MeshConverter::Convert(file, out_file);
  }
}

void MeshConvertToolWindow::Draw() {
  auto &renderer = Window::GetInstance().GetGraphics(window_id_)->renderer();
  renderer->MakeCurrent();
  renderer->ApplyTarget();
  DrawGUI();
  renderer->Render();
}

void MeshConvertToolWindow::DrawGUI() {
  auto &window_config = Window::GetInstance().Get(window_id_)->config();
  nuklear_->BeginFrame();
  auto ctx = nuklear_->context();
  nk_begin(ctx, "", nk_rect(-1, -1, window_config.width + 2, window_config.height + 2), NK_WINDOW_BACKGROUND);

  if (nk_tree_push(ctx, NK_TREE_NODE, "PostProcessFlag", NK_MAXIMIZED)) {
    nk_layout_row_dynamic(ctx, 30, 3);
    nk_checkbox_label(ctx, "calculate tangent space", &MeshConverter::post_process_flag.calc_tangent_space);
    nk_checkbox_label(ctx, "join identical vertices", &MeshConverter::post_process_flag.join_identical_vertices);
    nk_checkbox_label(ctx, "make left handed", &MeshConverter::post_process_flag.make_left_handed);
    nk_checkbox_label(ctx, "triangulate", &MeshConverter::post_process_flag.triangulate);
    nk_checkbox_label(ctx, "remove component", &MeshConverter::post_process_flag.remove_component);
    nk_checkbox_label(ctx, "generate normals", &MeshConverter::post_process_flag.gen_normals);
    nk_checkbox_label(ctx, "generate smooth normals", &MeshConverter::post_process_flag.gen_smooth_normals);
    nk_checkbox_label(ctx, "split large meshes", &MeshConverter::post_process_flag.split_large_meshes);
    nk_checkbox_label(ctx, "transform vertices", &MeshConverter::post_process_flag.pre_transform_vertices);
    nk_checkbox_label(ctx, "limit bone weights", &MeshConverter::post_process_flag.limit_bone_weights);
    nk_checkbox_label(ctx, "validate data structure", &MeshConverter::post_process_flag.validate_data_structure);
    nk_checkbox_label(ctx, "improve cache locality", &MeshConverter::post_process_flag.improve_cache_locality);
    nk_checkbox_label(ctx, "remove redundant materials", &MeshConverter::post_process_flag.remove_redundant_materials);
    nk_checkbox_label(ctx, "fix infacing normals", &MeshConverter::post_process_flag.fix_infacing_normals);
    nk_checkbox_label(ctx, "sort by topology type", &MeshConverter::post_process_flag.sort_by_ptype);
    nk_checkbox_label(ctx, "find degenerates", &MeshConverter::post_process_flag.find_degenerates);
    nk_checkbox_label(ctx, "find invalid data", &MeshConverter::post_process_flag.find_invalid_data);
    nk_checkbox_label(ctx, "gen uv coords", &MeshConverter::post_process_flag.gen_uv_coords);
    nk_checkbox_label(ctx, "transform uv coords", &MeshConverter::post_process_flag.transform_uv_coords);
    nk_checkbox_label(ctx, "find instances", &MeshConverter::post_process_flag.find_instances);
    nk_checkbox_label(ctx, "optimize meshes", &MeshConverter::post_process_flag.optimize_meshes);
    nk_checkbox_label(ctx, "optimize graph", &MeshConverter::post_process_flag.optimize_graph);
    nk_checkbox_label(ctx, "flip uvs", &MeshConverter::post_process_flag.flip_uvs);
    nk_checkbox_label(ctx, "flip winding order", &MeshConverter::post_process_flag.flip_winding_order);
    nk_checkbox_label(ctx, "split by bone count", &MeshConverter::post_process_flag.split_by_bone_count);
    nk_checkbox_label(ctx, "debone", &MeshConverter::post_process_flag.debone);
    nk_tree_pop(ctx);
  }

  if (nk_tree_push(ctx, NK_TREE_NODE, "RemoveComponentFlag", NK_MINIMIZED)) {
    nk_layout_row_dynamic(ctx, 30, 3);
    nk_checkbox_label(ctx, "normals", &MeshConverter::remove_component_flag.normals);
    nk_checkbox_label(ctx, "tangents & bitangents", &MeshConverter::remove_component_flag.tangents_and_bitangents);
    nk_checkbox_label(ctx, "colors", &MeshConverter::remove_component_flag.colors);
    nk_checkbox_label(ctx, "colors 0", &MeshConverter::remove_component_flag.colors0);
    nk_checkbox_label(ctx, "colors 1", &MeshConverter::remove_component_flag.colors1);
    nk_checkbox_label(ctx, "colors 2", &MeshConverter::remove_component_flag.colors2);
    nk_checkbox_label(ctx, "colors 3", &MeshConverter::remove_component_flag.colors3);
    nk_checkbox_label(ctx, "texcoords", &MeshConverter::remove_component_flag.texcoords);
    nk_checkbox_label(ctx, "texcoords 0", &MeshConverter::remove_component_flag.texcoords0);
    nk_checkbox_label(ctx, "texcoords 1", &MeshConverter::remove_component_flag.texcoords1);
    nk_checkbox_label(ctx, "texcoords 2", &MeshConverter::remove_component_flag.texcoords2);
    nk_checkbox_label(ctx, "texcoords 3", &MeshConverter::remove_component_flag.texcoords3);
    nk_checkbox_label(ctx, "boneweights", &MeshConverter::remove_component_flag.boneweights);
    nk_checkbox_label(ctx, "animations", &MeshConverter::remove_component_flag.animations);
    nk_checkbox_label(ctx, "textures", &MeshConverter::remove_component_flag.textures);
    nk_checkbox_label(ctx, "lights", &MeshConverter::remove_component_flag.lights);
    nk_checkbox_label(ctx, "cameras", &MeshConverter::remove_component_flag.cameras);
    nk_checkbox_label(ctx, "meshes", &MeshConverter::remove_component_flag.meshes);
    nk_checkbox_label(ctx, "materials", &MeshConverter::remove_component_flag.materials);
    nk_tree_pop(ctx);
  }

  if (nk_tree_push(ctx, NK_TREE_NODE, "RemoveComponentFlag", NK_MINIMIZED)) {
    nk_layout_row_dynamic(ctx, 30, 3);
    nk_checkbox_label(ctx, "point", &MeshConverter::remove_topology_flag.point);
    nk_checkbox_label(ctx, "line", &MeshConverter::remove_topology_flag.line);
    nk_checkbox_label(ctx, "triangle", &MeshConverter::remove_topology_flag.triangle);
    nk_checkbox_label(ctx, "polygon", &MeshConverter::remove_topology_flag.polygon);
    nk_tree_pop(ctx);
  }

  nk_end(ctx);
  nuklear_->EndFrame();
}

} // namespace xEngine
