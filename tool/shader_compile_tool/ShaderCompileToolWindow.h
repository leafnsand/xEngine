#ifndef XENGINE_TOOL_SHADERCOMPILETTOOL_SHADERCOMPILETTOOLWINDOW_H
#define XENGINE_TOOL_SHADERCOMPILETTOOL_SHADERCOMPILETTOOLWINDOW_H

#include <resource/Resource.h>
#include <nuklear/NuklearGUI.h>
#include <window/WindowDelegate.h>

namespace xEngine {

  class ShaderCompileToolWindow : public WindowDelegate {
  public:
    ShaderCompileToolWindow();

    virtual ~ShaderCompileToolWindow();

    virtual void OnWindowUpdate() override;

    virtual void OnWindowClose() override;

    virtual void OnWindowDropFile(eastl::vector<eastl::string> files) override;

  private:
    void Draw();

    void DrawGUI();

  private:
    ResourceID window_id_{ kInvalidResourceID };
    NuklearGUIUniquePtr nuklear_{ nullptr };
  };

} // namespace xEngine

#endif // XENGINE_TOOL_SHADERCOMPILETTOOL_SHADERCOMPILETTOOLWINDOW_H