#if X_WINDOWS

#include "graphics/config/GraphicsConfig.h"

#include "core/Core.h"

#include <EASTL/allocator.h>

#include <Windows.h>

#pragma comment(lib, "opengl32.lib")

namespace xEngine {

struct Win32OpenGLContext{
  HDC dc{nullptr};
  HGLRC rc{nullptr};
  HWND hwnd{nullptr};
};

void *CreateOpenGLContext(const GraphicsConfig &config, void *native_handle) {
  auto context = static_cast<Win32OpenGLContext *>(eastl::GetDefaultAllocator()->allocate(sizeof(Win32OpenGLContext)));
  context->hwnd = static_cast<HWND>(native_handle);
  context->dc = GetDC(context->hwnd);
  PIXELFORMATDESCRIPTOR pixel_format_desc;
  ZeroMemory(&pixel_format_desc, sizeof(PIXELFORMATDESCRIPTOR));
  pixel_format_desc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  pixel_format_desc.nVersion = 1;
  pixel_format_desc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pixel_format_desc.iPixelType = PFD_TYPE_RGBA;
  pixel_format_desc.cColorBits = ChannelBitOfPixelFormat(config.color_format, PixelChannel::kRed)
                               + ChannelBitOfPixelFormat(config.color_format, PixelChannel::kGreen)
                               + ChannelBitOfPixelFormat(config.color_format, PixelChannel::kBlue);
  pixel_format_desc.cRedBits = ChannelBitOfPixelFormat(config.color_format, PixelChannel::kRed);
  pixel_format_desc.cGreenBits = ChannelBitOfPixelFormat(config.color_format, PixelChannel::kGreen);
  pixel_format_desc.cBlueBits = ChannelBitOfPixelFormat(config.color_format, PixelChannel::kBlue);
  pixel_format_desc.cAlphaBits = ChannelBitOfPixelFormat(config.color_format, PixelChannel::kAlpha);
  pixel_format_desc.cDepthBits = ChannelBitOfPixelFormat(config.depth_format, PixelChannel::kDepth);
  pixel_format_desc.cStencilBits = ChannelBitOfPixelFormat(config.depth_format, PixelChannel::kStencil);
  pixel_format_desc.iLayerType = PFD_MAIN_PLANE;
  auto pixel_format = ChoosePixelFormat(context->dc, &pixel_format_desc);
  SetPixelFormat(context->dc, pixel_format, &pixel_format_desc);
  context->rc = wglCreateContext(context->dc);
  return context;
}

void DestroyOpenGLContext(void *value) {
  auto context = static_cast<Win32OpenGLContext *>(value);
  wglDeleteContext(context->rc);
  ReleaseDC(context->hwnd, context->dc);
  eastl::GetDefaultAllocator()->deallocate(value, sizeof(Win32OpenGLContext));
}

void OpenGLContextSwapBuffers(void *value) {
  auto context = static_cast<Win32OpenGLContext *>(value);
  SwapBuffers(context->dc);
}

void OpenGLContextMakeCurrent(void *value) {
  auto context = static_cast<Win32OpenGLContext *>(value);
  wglMakeCurrent(context->dc, context->rc);
}

void OpenGLContextSetSwapInterval(void *value, int32 interval) {
  auto context = static_cast<Win32OpenGLContext *>(value);
  auto wglSwapIntervalEXT = reinterpret_cast<BOOL(*)(int)>(wglGetProcAddress("wglSwapIntervalEXT"));
  if (wglSwapIntervalEXT != nullptr) {
    wglSwapIntervalEXT(interval);
  }
}

} // namespace xEngine

#endif // X_WINDOWS