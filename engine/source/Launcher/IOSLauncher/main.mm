#import <UIKit/UIKit.h>

int main(int argc, char* argv[])
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
    UIApplicationMain(argc,
                      argv,
                      @"IOSApplication",
                      @"IOSApplicationDelegate");
    [pool release];
    return 0;
}
