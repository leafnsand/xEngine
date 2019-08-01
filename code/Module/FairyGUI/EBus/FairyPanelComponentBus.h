#pragma once

#include "AzCore/EBus/EBus.h"
#include "Renderer/Base/Material.h"
#include "FairyGUI/fairy/fairytypes.h"
#include "FairyGUI/fairy/ui/GComponent.h"

namespace Module
{
    class FairyPanelRequest : public AZ::ComponentBus
    {
    public:

        virtual ~FairyPanelRequest() {}
        // old
        virtual void SetMaterialAndMesh(AZStd::shared_ptr<Module::Material> material, int subMeshIndex) = 0;
        // old
        virtual void ClearMaterialAndMesh() = 0;

        virtual void SetConfigData(AZStd::string packageName, AZStd::string componentName) = 0;

        virtual void SetRenderMode(fairygui::FairyRenderMode renderMode) = 0;

        virtual void SetSortingOrder(int value, bool apply) = 0;

        virtual void CreateUI() = 0;

        virtual fairygui::GComponent* GetComponent() = 0;

    };

    using FairyPanelRequestBus = AZ::EBus<FairyPanelRequest>;

    class FairyPanelNotification : public AZ::ComponentBus
    {
    public:

        virtual ~FairyPanelNotification() {}
    };

    using FairyPanelNotificationBus = AZ::EBus<FairyPanelNotification>;
}
