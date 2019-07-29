#include "IOSApplication.h"

#include <AzCore/IO/SystemFile.h>

#include <AzCore/API/ApplicationAPI_ios.h>
#include <AzCore/Application/Application.h>
#include <AzCore/Module/Module.h>

extern void CreateStaticModules(AZStd::vector<AZ::Module*>& modules);

AZ::Application* gApp;

////////////////////////////////////////////////////////////////////////////////////////////////////
@implementation IOSApplicationDelegate

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)startMainLoop
{
    AZ::Application app;
    gApp = &app;
    
    char pathToAssets[AZ_MAX_PATH_LEN] = { 0 };
    const char* pathToResources = [[[NSBundle mainBundle] resourcePath] UTF8String];
    azsnprintf(pathToAssets, AZ_MAX_PATH_LEN, "%s/%s", pathToResources, "assets");

    char pathToDescriptor[AZ_MAX_PATH_LEN] = { 0 };
    azsnprintf(pathToDescriptor, AZ_MAX_PATH_LEN, "%s/%s", pathToAssets, "game.xml");
    
    AZ::Application::StartupParameters params;
    params.m_createStaticModulesCallback = CreateStaticModules;
    params.m_appRootOverride = pathToAssets;
    params.m_loadDynamicModules = false;
    
    app.Start(pathToDescriptor, params);
    app.RunMainLoop();
    app.Stop();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
    [self performSelector:@selector(startMainLoop) withObject:nil afterDelay:0.0];
    return YES;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)applicationWillResignActive:(UIApplication*)application
{
    EBUS_EVENT(AZ::IosLifecycleEvents::Bus, OnWillResignActive);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)applicationDidEnterBackground:(UIApplication*)application
{
    EBUS_EVENT(AZ::IosLifecycleEvents::Bus, OnDidEnterBackground);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)applicationWillEnterForeground:(UIApplication*)application
{
    EBUS_EVENT(AZ::IosLifecycleEvents::Bus, OnWillEnterForeground);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)applicationDidBecomeActive:(UIApplication*)application
{
    EBUS_EVENT(AZ::IosLifecycleEvents::Bus, OnDidBecomeActive);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)applicationWillTerminate:(UIApplication *)application
{
    EBUS_EVENT(AZ::IosLifecycleEvents::Bus, OnWillTerminate);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application
{
    EBUS_EVENT(AZ::IosLifecycleEvents::Bus, OnDidReceiveMemoryWarning);
}

@end // IOSApplicationDelegate Implementation
