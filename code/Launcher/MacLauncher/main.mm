#include "MacApplication.h"

#include <AzCore/API/ApplicationAPI_darwin.h>
#include <AzCore/Application/Application.h>
#include <AzCore/Module/Module.h>

AZ::Application* gApp;

extern void CreateStaticModules(AZStd::vector<AZ::Module*>& modules);

class ApplicationLifecycleHandler : public AZ::DarwinLifecycleEvents::Bus::Handler
{
public:
    ApplicationLifecycleHandler() { BusConnect(); }
    ~ApplicationLifecycleHandler() { BusDisconnect(); }
    void OnWillTerminate() override { gApp->Stop(); }
};

int main(int argc, char* argv[])
{
    // Ensure the process is a foreground application. Must be done before creating the application.
    ProcessSerialNumber processSerialNumber = { 0, kCurrentProcess };
    TransformProcessType(&processSerialNumber, kProcessTransformToForegroundApplication);

    // Create a memory pool, a custom AppKit application, and a custom AppKit application delegate.
    NSAutoreleasePool* autoreleasePool = [[NSAutoreleasePool alloc] init];
    [MacApplication sharedApplication];
    [NSApp setDelegate: [[MacApplicationDelegate alloc] init]];

    // Register some default application behaviours
    [[NSUserDefaults standardUserDefaults] registerDefaults:
        [[NSDictionary alloc] initWithObjectsAndKeys:
            [NSNumber numberWithBool: FALSE], @"AppleMomentumScrollSupported",
            [NSNumber numberWithBool: FALSE], @"ApplePressAndHoldEnabled",
            nil]];

    // Launch the AppKit application and release the memory pool.
    [NSApp finishLaunching];
    [autoreleasePool release];

    id quitMenuItem = [NSMenuItem new];
    [quitMenuItem initWithTitle:@"Quit"
                  action:@selector(terminate:)
                  keyEquivalent:@"q"];

    id appMenu = [NSMenu new];
    [appMenu addItem:quitMenuItem];

    id appMenuItem = [NSMenuItem new];
    [appMenuItem setSubmenu:appMenu];

    id menubar = [[NSMenu new] autorelease];
    [menubar addItem:appMenuItem];
    [NSApp setMainMenu:menubar];

    AZ::Application application(&argc, &argv);
    gApp = &application;

    char pathToAssets[AZ_MAX_PATH_LEN] = { 0 };
    const char* pathToResources = [[[NSBundle mainBundle] resourcePath] UTF8String];
    azsnprintf(pathToAssets, AZ_MAX_PATH_LEN, "%s/%s", pathToResources, "assets");

    char pathToDescriptor[AZ_MAX_PATH_LEN] = { 0 };
    azsnprintf(pathToDescriptor, AZ_MAX_PATH_LEN, "%s/%s", pathToAssets, "game.xml");

    AZ::Application::StartupParameters params;
    params.m_createStaticModulesCallback = CreateStaticModules;
    params.m_appRootOverride = pathToAssets;
    params.m_loadDynamicModules = false;

    ApplicationLifecycleHandler handler;

    application.Start(pathToDescriptor, params);
    application.RunMainLoop();
    application.Stop();

    return 0;
}