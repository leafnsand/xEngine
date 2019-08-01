#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_APPLE_OSX)

#include <AppKit/AppKit.h>

void* CreateNativeWindow(int& width, int& height)
{
    CGRect screenBounds = CGRectMake(0, 0, width, height);
    NSUInteger styleMask = NSClosableWindowMask|NSTitledWindowMask|NSMiniaturizableWindowMask;
    NSWindow* nativeWindow = [[NSWindow alloc] initWithContentRect: screenBounds styleMask: styleMask backing: NSBackingStoreBuffered defer:false];
    [nativeWindow makeKeyAndOrderFront:nativeWindow];
    return nativeWindow;
}

#endif