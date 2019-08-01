#include "IOSApplication.h"

#include <AzCore/Input/Buses/Notifications/RawInputNotificationBus_ios.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
@implementation IOSApplication

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)touchesBegan: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
    for (const UITouch* touch in touches)
    {
        EBUS_EVENT(AZ::RawInputNotificationBusIos, OnRawTouchEventBegan, touch);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)touchesMoved: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
    for (const UITouch* touch in touches)
    {
        EBUS_EVENT(AZ::RawInputNotificationBusIos, OnRawTouchEventMoved, touch);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)touchesEnded: (NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
    for (const UITouch* touch in touches)
    {
        EBUS_EVENT(AZ::RawInputNotificationBusIos, OnRawTouchEventEnded, touch);

    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent: (UIEvent*)event
{
    // Active touches can be cancelled (as opposed to ended) for a variety of reasons, including:
    // - The active view being rotated to match the device orientation.
    // - The application resigning it's active status (eg. when receiving a message or phone call).
    // - Exceeding the max number of active touches tracked by the system (which as explained above
    //   is device dependent). For some reason this causes all active touches to be cancelled.
    // In any case, for the purposes of a game (or really any application that I can think of),
    // there really isn't any reason to distinguish between a touch ending or being cancelled.
    // They are mutually exclusive events, and both result in the touch being discarded by the
    // system.
    [self touchesEnded: touches withEvent: event];
}

@end // IOSApplication Implementation