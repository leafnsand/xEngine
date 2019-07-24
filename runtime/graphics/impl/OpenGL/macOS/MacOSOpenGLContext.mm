#if X_MACOS

#include "graphics/config/GraphicsConfig.h"

#include "core/Core.h"

#import <Cocoa/Cocoa.h>

namespace xEngine {

void *CreateOpenGLContext(const GraphicsConfig &config, void *native_handle) {
  NSWindow *window = static_cast<NSWindow *>(native_handle);
  int32 attributes[] = {
    NSOpenGLPFAAccelerated, NSOpenGLPFAClosestPolicy, NSOpenGLPFADoubleBuffer,
    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion4_1Core,
    NSOpenGLPFAColorSize, ChannelBitOfPixelFormat(config.color_format, PixelChannel::kRed)
                        + ChannelBitOfPixelFormat(config.color_format, PixelChannel::kGreen)
                        + ChannelBitOfPixelFormat(config.color_format, PixelChannel::kBlue),
    NSOpenGLPFAAlphaSize, ChannelBitOfPixelFormat(config.color_format, PixelChannel::kAlpha),
    NSOpenGLPFADepthSize, ChannelBitOfPixelFormat(config.depth_format, PixelChannel::kDepth),
    NSOpenGLPFAStencilSize, ChannelBitOfPixelFormat(config.depth_format, PixelChannel::kStencil),
    NSOpenGLPFASampleBuffers, config.sample_count == 0 ? 0 : 1,
    NSOpenGLPFASamples, config.sample_count,
    0
  };
  NSOpenGLPixelFormat *pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:(NSOpenGLPixelFormatAttribute *)attributes];
  x_assert(pixel_format != nullptr);
  NSOpenGLContext *context = [[NSOpenGLContext alloc] initWithFormat:pixel_format shareContext:nil];
  [context setView:[window contentView]];
  [pixel_format release];
  return context;
}

void DestroyOpenGLContext(void *value) {
  NSOpenGLContext *context = static_cast<NSOpenGLContext *>(value);
  [context release];
}

void OpenGLContextSwapBuffers(void *value) {
  NSOpenGLContext *context = static_cast<NSOpenGLContext *>(value);
  [context flushBuffer];
}

void OpenGLContextMakeCurrent(void *value) {
  NSOpenGLContext *context = static_cast<NSOpenGLContext *>(value);
  [context makeCurrentContext];
}

void OpenGLContextSetSwapInterval(void *value, int32 interval) {
  NSOpenGLContext *context = static_cast<NSOpenGLContext *>(value);
  [context setValues:&interval forParameter:NSOpenGLCPSwapInterval];
}

} // namespace xEngine

#endif