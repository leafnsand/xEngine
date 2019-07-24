#if X_MACOS

#include "application/ApplicationDelegate.h"

#import <Cocoa/Cocoa.h>

using namespace xEngine;

@interface MacOSAppDelegate : NSObject <NSApplicationDelegate> {
  ApplicationDelegate *delegate_;
}

- (id) initWithApplicationDelegate: (ApplicationDelegate *) delegate;

@end

@implementation MacOSAppDelegate

- (id) initWithApplicationDelegate: (ApplicationDelegate *) delegate {
  self = [super init];
  if (self != nil) delegate_ = delegate;
  return self;
}

- (void) applicationDidFinishLaunching: (NSNotification *) notification {
  if (delegate_ != nullptr) {
    delegate_->Initialize();
  }
  [[NSApplication sharedApplication] stop:self];
}

- (void) applicationWillTerminate: (NSNotification *) notification {
  if (delegate_ != nullptr) {
    delegate_->Finalize();
  }
}

@end

namespace xEngine {

void NativeApplicationInitialize(ApplicationDelegate *delegate) {
  @autoreleasepool {
    [NSApplication sharedApplication];
    [[NSApplication sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSMenu *main_menu = [[NSMenu alloc] init];
    NSMenu *menu = [[NSMenu alloc] init];

    NSMenuItem *main_menu_item = [main_menu addItemWithTitle:@"" action:nil keyEquivalent:@""];
    [main_menu_item setSubmenu:menu];
    [[NSApplication sharedApplication] setMainMenu:main_menu];

    [menu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];

    [[NSApplication sharedApplication] performSelector:NSSelectorFromString(@"setAppleMenu:") withObject:menu];
    [[NSApplication sharedApplication] setDelegate:[[MacOSAppDelegate alloc] initWithApplicationDelegate:delegate]];
    [[NSApplication sharedApplication] run];
  }
}

void NativeApplicationFinalize(ApplicationDelegate *delegate) {
  [[NSApplication sharedApplication] terminate:nil];
}

bool PollNativeApplicationEvent() {
  @autoreleasepool {
    NSEvent *event;
    do {
      event = [[NSApplication sharedApplication] nextEventMatchingMask:NSEventMaskAny
                                                 untilDate:[NSDate distantPast]
                                                 inMode:NSDefaultRunLoopMode
                                                 dequeue:YES];
      [[NSApplication sharedApplication] sendEvent: event];
    } while(event != nil);
    return false;
  }
}

} // namespace xEngine

#endif // X_MACOS
