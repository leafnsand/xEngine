#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Component/ComponentBus.h>

namespace fairygui
{
    class NGraphics;
}

namespace Module
{
    class FairyMeshRequest : public AZ::ComponentBus
    {
    public:
        virtual ~FairyMeshRequest() {}

        virtual void RefreshData( fairygui::NGraphics* graphics) = 0;
    };

    using FairyMeshRequestBus = AZ::EBus<FairyMeshRequest>;

    class FairyMeshNotification : public AZ::ComponentBus
    {
    public:
        virtual ~FairyMeshNotification() {}
    };

    using FairyMeshNotificationBus = AZ::EBus<FairyMeshNotification>;
}
