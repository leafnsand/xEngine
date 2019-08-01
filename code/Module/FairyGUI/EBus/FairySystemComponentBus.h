#pragma once

#include <AzCore/EBus/EBus.h>
#include "AzCore/RTTI/TypeInfo.h"
#include "FairyGUI/fairy/fairytypes.h"
#include "FairyGUI/fairy/FairyGUI.h"
#include "Renderer/Base/Material.h"

using namespace fairygui;

namespace Module
{
    class FairySystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual void ChangeStageCameraPos(float posX, float posY) = 0;

        virtual float GetPanelScaleValue() = 0;

        virtual ~FairySystemRequest() {}
    };

    using FairySystemRequestBus = AZ::EBus<FairySystemRequest>;

    class FairyPackageSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~FairyPackageSystemRequest() {}

        virtual void AddPackage(const AZStd::string& url, AddPackageCallBack callback) = 0;

        virtual GRoot* GetGRoot() = 0;

        virtual GObject* CreateObject(const AZStd::string& pkgName, const AZStd::string& resName) = 0;

        virtual GObject* CreateSpine(const AZStd::string& spineName) = 0;

        virtual GObject* CreateParticle2d(const AZStd::string& particleName) = 0;

        virtual Ref* CreateEmptyObject(const AZStd::string& typeName) = 0;

        virtual AZStd::string GetPackageItemURL(const AZStd::string& pkgName, const AZStd::string& resName) = 0;

        virtual AZ::EntityId CreateUIPanel(const AZStd::string& pkgName = "", const AZStd::string& componentName = "") = 0;
    };

    using FairyPackageSystemRequestBus = AZ::EBus<FairyPackageSystemRequest>;

    class FairySystemNotification : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~FairySystemNotification() {}
    };

    using FairySystemNotificationBus = AZ::EBus<FairySystemNotification>;
}
