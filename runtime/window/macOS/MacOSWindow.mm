#if X_MACOS

#include "MacOSWindow.h"

#include "application/Application.h"

#include <EASTL/allocator.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>

#import <Cocoa/Cocoa.h>

using namespace xEngine;

@interface MacOSWindowDelegate : NSObject <NSWindowDelegate> {
  MacOSWindow *window;
}

- (id) initWithMacOSWindow: (MacOSWindow *) initWindow;

@end

@implementation MacOSWindowDelegate

- (id) initWithMacOSWindow: (MacOSWindow *) initWindow {
  self = [super init];
  if (self != nil) window = initWindow;
  return self;
}

- (BOOL) windowShouldClose: (id) sender {
  GetConfig(window).delegate->OnWindowClose();
  return NO;
}

@end

@interface MacOSWindowView : NSView {
  MacOSWindow *window;
}

- (id) initWithMacOSWindow: (MacOSWindow *) initWindow;

@end

@implementation MacOSWindowView

- (id) initWithMacOSWindow: (MacOSWindow *) initWindow {
  @autoreleasepool {
    self = [super init];
    if (self != nil) {
      window = initWindow;
      [self registerForDraggedTypes:[NSArray arrayWithObjects:NSFilenamesPboardType, nil]];
    }
    return self;
  }
}

- (BOOL) isOpaque {
  return YES;
}

- (BOOL) canBecomeKeyView {
  return YES;
}

- (BOOL) acceptsFirstResponder {
  return YES;
}

- (void) viewDidChangeBackingProperties {
  @autoreleasepool {
    const NSRect content_rect = [self frame];
    const NSRect frame_buffer_rect = [self convertRectToBacking:content_rect];

    auto &config = GetConfig(window);
    config.frame_buffer_width = frame_buffer_rect.size.width;
    config.frame_buffer_height = frame_buffer_rect.size.height;

    GetConfig(window).delegate->OnWindowResize();
  }
}

- (void) mouseMoved: (NSEvent *) event {
  @autoreleasepool {
    const NSRect content_rect = [self frame];
    const NSPoint position = [event locationInWindow];
    SetMousePosition(window, position.x, content_rect.size.height - position.y);
   }
}

- (void) mouseDragged: (NSEvent *) event {
  [self mouseMoved:event];
}

- (void) rightMouseDragged: (NSEvent *) event {
  [self mouseMoved:event];
}

- (void) otherMouseDragged: (NSEvent *) event {
  [self mouseMoved:event];
}

- (void) mouseDown: (NSEvent *) event {
  SetMouseButtonDown(window, MouseButtonType::kLeft);
}

- (void) mouseUp: (NSEvent *) event {
  SetMouseButtonUp(window, MouseButtonType::kLeft);
}

- (void) rightMouseDown: (NSEvent *) event {
  SetMouseButtonDown(window, MouseButtonType::kRight);
}

- (void) rightMouseUp: (NSEvent *) event {
  SetMouseButtonUp(window, MouseButtonType::kRight);
}

- (void) otherMouseDown: (NSEvent *) event {
  SetMouseButtonDown(window, MouseButtonType::kMiddle);
}

- (void) otherMouseUp: (NSEvent *) event {
  SetMouseButtonUp(window, MouseButtonType::kMiddle);
}

- (void) scrollWheel: (NSEvent *) event {
  @autoreleasepool {
    auto x = [event scrollingDeltaX];
    auto y = [event scrollingDeltaY];
    if ([event hasPreciseScrollingDeltas]) {
        x *= 0.1;
        y *= 0.1;
    }
    if (fabs(x) > 0.0 || fabs(y) > 0.0) {
      SetMouseScroll(window, x, y);
    }
  }
}

- (NSDragOperation) draggingEntered: (id <NSDraggingInfo>) sender {
  if ((NSDragOperationGeneric & [sender draggingSourceOperationMask]) == NSDragOperationGeneric) {
    [self setNeedsDisplay:YES];
    return NSDragOperationGeneric;
  }
  return NSDragOperationNone;
}

- (BOOL) performDragOperation: (id <NSDraggingInfo>) sender {
  @autoreleasepool {
    const NSRect content_rect = [self frame];
    const NSPoint position = [sender draggingLocation];
    SetMousePosition(window, position.x, content_rect.size.height - position.y);

    eastl::vector<eastl::string> vector;
    NSPasteboard *pasteboard = [sender draggingPasteboard];
    NSArray *files = [pasteboard propertyListForType:NSFilenamesPboardType];
    auto count = [files count];
    if (count > 0) {
      auto enumerator = [files objectEnumerator];
      for (auto i = 0; i < count; ++i) {
        vector.push_back(eastl::string([[enumerator nextObject] UTF8String]));
      }
    }
    GetConfig(window).delegate->OnWindowDropFile(vector);
    return YES;
  }
}

@end

namespace xEngine {

void MacOSWindow::Create(const WindowConfig &config) {
  @autoreleasepool {
    x_assert(config.delegate != nullptr);
    config_ = config;
    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, config.width, config.height)
                                         styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable)
                                         backing:NSBackingStoreBuffered
                                         defer:YES];
    [window setTitle:[NSString stringWithCString:config.title.c_str() encoding:[NSString defaultCStringEncoding]]];
    [window setDelegate:[[MacOSWindowDelegate alloc] initWithMacOSWindow:this]];
    MacOSWindowView *view = [[MacOSWindowView alloc] initWithMacOSWindow:this];
    [view setWantsBestResolutionOpenGLSurface:YES];
    [window makeFirstResponder:view];
    [window setContentView:view];
    [window setAcceptsMouseMovedEvents:YES];
    [window setRestorable:NO];
    [window orderFront:nil];
    [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
    [window makeKeyAndOrderFront:nil];
    [window center];
    window_ = window;
    Application::GetInstance().AddLoopDelegate(this);
  }
}

void MacOSWindow::Destroy() {
  @autoreleasepool {
    NSWindow *window = static_cast<NSWindow *>(window_);
    [[window contentView] release];
    [[window delegate] release];
    [window release];
    window_ = nullptr;
    Application::GetInstance().RemoveLoopDelegate(this);
  }
}

void MacOSWindow::SetTitle(const eastl::string &name) {
  @autoreleasepool {
    NSWindow *window = static_cast<NSWindow *>(window_);
    [window setTitle:[NSString stringWithCString:name.c_str() encoding:[NSString defaultCStringEncoding]]];
  }
}

void MacOSWindow::OnBeforeEventTick() {
  Reset();
}

void MacOSWindow::OnAfterEventTick() {
  config_.delegate->OnWindowUpdate();
}

} // namespace xEngine

#endif