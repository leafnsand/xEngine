#ifndef XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTTOOLWINDOW_H
#define XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTTOOLWINDOW_H

#include <resource/Resource.h>
#include <nuklear/NuklearGUI.h>
#include <window/WindowDelegate.h>

namespace xEngine {

class MeshConvertToolWindow : public WindowDelegate {
 public:
  MeshConvertToolWindow();

  virtual ~MeshConvertToolWindow();

  virtual void OnWindowUpdate() override;

  virtual void OnWindowClose() override;

  virtual void OnWindowDropFile(eastl::vector<eastl::string> files) override;

 private:
  void Draw();

  void DrawGUI();

 private:
  ResourceID window_id_{kInvalidResourceID};
  NuklearGUIUniquePtr nuklear_{nullptr};
};

} // namespace xEngine

#endif // XENGINE_TOOL_MESHCONVERTTOOL_MESHCONVERTTOOLWINDOW_H