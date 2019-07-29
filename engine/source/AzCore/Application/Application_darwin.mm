/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_APPLE_OSX)

#include <AzCore/API/ApplicationAPI_darwin.h>
#include <AzCore/Application/Application.h>
#include <AzCore/Input/Buses/Notifications/RawInputNotificationBus_darwin.h>

#include <AppKit/AppKit.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    class ApplicationDarwin
        : public Application::Implementation
        , public DarwinLifecycleEvents::Bus::Handler
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        AZ_CLASS_ALLOCATOR(ApplicationDarwin, AZ::SystemAllocator, 0);
        ApplicationDarwin();
        ~ApplicationDarwin() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // DarwinLifecycleEvents
        void OnWillResignActive() override;
        void OnDidBecomeActive() override;
        void OnDidResignActive() override;
        void OnWillBecomeActive() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Application::Implementation
        void PumpSystemEventLoopOnce() override;
        void PumpSystemEventLoopUntilEmpty() override;

    protected:
        bool ProcessNextSystemEvent(); // Returns true if an event was processed, false otherwise

    private:
        ApplicationLifecycleEvents::Event m_lastEvent;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    Application::Implementation* Application::Implementation::Create()
    {
        return aznew ApplicationDarwin();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Application::Implementation::GetPublicStoragePath()
    {
        return [[NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES) firstObject] UTF8String];
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ApplicationDarwin::ApplicationDarwin()
        : m_lastEvent(ApplicationLifecycleEvents::Event::None)
    {
        DarwinLifecycleEvents::Bus::Handler::BusConnect();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    ApplicationDarwin::~ApplicationDarwin()
    {
        DarwinLifecycleEvents::Bus::Handler::BusDisconnect();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ApplicationDarwin::OnWillResignActive()
    {
        EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationConstrained, m_lastEvent);
        m_lastEvent = ApplicationLifecycleEvents::Event::Constrain;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ApplicationDarwin::OnDidBecomeActive()
    {
        EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationUnconstrained, m_lastEvent);
        m_lastEvent = ApplicationLifecycleEvents::Event::Unconstrain;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ApplicationDarwin::OnDidResignActive()
    {
        EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationSuspended, m_lastEvent);
        m_lastEvent = ApplicationLifecycleEvents::Event::Suspend;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ApplicationDarwin::OnWillBecomeActive()
    {
        EBUS_EVENT(ApplicationLifecycleEvents::Bus, OnApplicationResumed, m_lastEvent);
        m_lastEvent = ApplicationLifecycleEvents::Event::Resume;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ApplicationDarwin::PumpSystemEventLoopOnce()
    {
        ProcessNextSystemEvent();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void ApplicationDarwin::PumpSystemEventLoopUntilEmpty()
    {
        bool eventProcessed = false;
        do
        {
            eventProcessed = ProcessNextSystemEvent();
        }
        while (eventProcessed);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool ApplicationDarwin::ProcessNextSystemEvent()
    {
        @autoreleasepool
        {
            NSEvent* event = [NSApp nextEventMatchingMask: NSAnyEventMask
                                    untilDate: [NSDate distantPast]
                                    inMode: NSDefaultRunLoopMode
                                    dequeue: YES];
            if (event != nil)
            {
                RawInputNotificationBusOsx::Broadcast(&RawInputNotificationsOsx::OnRawInputEvent, event);
                [NSApp sendEvent: event];
                return true;
            }
            else
            {
                return false;
            }
        }
    }
} // namespace AZ

#endif // defined(AZ_PLATFORM_APPLE_OSX)
