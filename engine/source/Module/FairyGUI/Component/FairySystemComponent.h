#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <Renderer/EBus/RendererSystemComponentBus.h>
#include <FairyGUI/EBus/FairySystemComponentBus.h>
#include <FairyGUI/Asset/FairyAssetHandler.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>
#include <AzCore/Input/Events/InputChannelEventListener.h>
#include <Window/EBus/WindowSystemComponentBus.h>
#include "AzCore/Component/Entity.h"

#include <AzCore/RTTI/TypeInfo.h>

namespace Module
{
    class FairySystemComponent
        : public AZ::Component
        , protected FairySystemRequestBus::Handler
        , protected AZ::SystemTickBus::Handler
        //, protected RendererSystemNotificationBus::Handler
        , protected FairyPackageSystemRequestBus::Handler
        , public AZ::InputChannelEventListener
        , protected WindowsSystemNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(FairySystemComponent, "{276AE61A-315B-431F-9BE5-8CB2B24480F3}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("FairyService"));
            provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("FairyService"));
            incompatible.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("RendererSystemService"));
            dependent.push_back(AZ_CRC("AssetDatabaseService"));
            dependent.push_back(AZ_CRC("FontService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("RendererSystemService"));
            required.push_back(AZ_CRC("AssetDatabaseService"));
            required.push_back(AZ_CRC("FontService"));
        }

        static void Reflect(AZ::ReflectContext* reflection);

        FairySystemComponent();
        FairySystemComponent(const FairySystemComponent&) = delete;
        virtual ~FairySystemComponent() = default;

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // AZ::SystemTickBus::Handler
        void OnSystemTick() override;

        //RendererSystemNotificationBus::Handler
        //void OnPreRender() override;
        //void OnRender() override;
        //void OnPostRender() override;

        // FairySystemRequestBus::Handler
        void ChangeStageCameraPos(float posX, float posY) override;
        float GetPanelScaleValue() override;

        // FairyPackageSystemRequestBus::Handler
        void AddPackage(const AZStd::string& url, AddPackageCallBack callback) override;
        GRoot* GetGRoot() override;
        GObject* CreateObject(const AZStd::string& pkgName, const AZStd::string& resName) override;
        GObject* CreateSpine(const AZStd::string& spineName) override;
        GObject* CreateParticle2d(const AZStd::string& particleName) override;
        Ref* CreateEmptyObject(const AZStd::string& typeName) override;
        AZStd::string GetPackageItemURL(const AZStd::string& pkgName, const AZStd::string& resName) override;
        AZ::EntityId CreateUIPanel(const AZStd::string& pkgName = "", const AZStd::string& componentName = "") override;

        //InputChannelEventListener
        bool OnInputChannelEventFiltered(const AZ::InputChannel& inputChannel) override;

        //WindowsSystemNotificationBus
        void OnWindowSizeChanged(int width, int height) override;

        // MeshProviderNotificationBus::Handler

    private:
        void InitFairyRoot();
        void DestroyFairyRoot();
        int RegisterInput(InputProcessor* pInput);
        void UnRegisterInput(int id);

        void CreateStageCamera();
        void DestroyStageCamera();

        AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>> m_assetHandlers;
        float m_defaultWidth;
        float m_defaultHeight;

        int m_subMeshIndex;

        float m_panelScale = 0;

        int checkIndex = 0;
    };
}
