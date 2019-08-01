#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_APPLE_IOS)

#include <UIKit/UIKit.h>

@interface IOSView : UIView

@end

@implementation IOSView

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

@end

void* CreateNativeWindow(int& width, int& height)
{
    CGRect mainScreenBounds = [[UIScreen mainScreen] bounds];

    UIWindow* mainWindow = [[UIWindow alloc] initWithFrame:mainScreenBounds];
    mainWindow.backgroundColor = [UIColor blackColor];

    IOSView* mainView = [[IOSView alloc] initWithFrame:mainScreenBounds];
    [mainWindow addSubview:mainView];

    UIViewController* viewController = [[UIViewController alloc] init];
    viewController.view = mainView;

    [mainWindow setRootViewController:viewController];
    [mainWindow makeKeyAndVisible];

    float scale = [[UIScreen mainScreen] scale];
    [mainView setContentScaleFactor:scale];

    width = scale * mainScreenBounds.size.width;
    height = scale * mainScreenBounds.size.height;

    return mainView.layer;
}

#endif
