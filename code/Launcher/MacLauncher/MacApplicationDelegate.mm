#include "MacApplication.h"

#include <AzCore/API/ApplicationAPI_darwin.h>

@implementation MacApplicationDelegate

- (void)applicationWillBecomeActive: (NSNotification*)notification
{
    EBUS_EVENT(AZ::DarwinLifecycleEvents::Bus, OnWillBecomeActive);
}

- (void)applicationDidBecomeActive: (NSNotification*)notification
{
    EBUS_EVENT(AZ::DarwinLifecycleEvents::Bus, OnDidBecomeActive);
}

- (void)applicationWillResignActive: (NSNotification*)notification
{
    EBUS_EVENT(AZ::DarwinLifecycleEvents::Bus, OnWillResignActive);
}

- (void)applicationDidResignActive: (NSNotification*)notification
{
    EBUS_EVENT(AZ::DarwinLifecycleEvents::Bus, OnDidResignActive);
}

- (void)applicationWillTerminate: (NSNotification*)notification
{
    EBUS_EVENT(AZ::DarwinLifecycleEvents::Bus, OnWillTerminate);
}

@end // MacApplicationDelegate Implementation