#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/Component/TransformBus.h"
#include "FairyGUI/EBus/FairyPanelComponentBus.h"

#include "bgfx/bgfx.h"

#include "Renderer/EBus/MeshProviderBus.h"
#include "FairyGUI/fairy/render/RenderDataProcess.h"
#include <Renderer/EBus/RendererSystemComponentBus.h>
#include "FairyGUI/fairy/render/BatchNode.h"
#include "FairyGUI/fairy/display/Container.h"

namespace Module
{
    class FairyPanelComponent
        : public AZ::Component
        , protected AZ::TransformNotificationBus::Handler
        , protected FairyPanelRequestBus::Handler
        , protected MeshProviderRequestBus::Handler
        , protected RendererSystemNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(FairyPanelComponent, "{C95EBC97-CCFA-4781-9A6F-FA14D3614CCC}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("FairyPanelService"));
            // provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("FairyPanelService"));
            // incompatible.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("TransformService"));
            // dependent.push_back(AZ_CRC("RendererService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("TransformService"));
            // required.push_back(AZ_CRC("RendererService"));
        }

        static void Reflect(AZ::ReflectContext* reflection);

        FairyPanelComponent() {};
        FairyPanelComponent(AZStd::string packageName, AZStd::string componentName) :
            m_packageName(packageName),
            m_componentName(componentName)
        {};

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

    protected:
        // AZ::TransformNotificationBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;

        // FairyPanelRequestBus::Handler
        // old
        void SetMaterialAndMesh(AZStd::shared_ptr<Module::Material> material, int subMeshIndex) override;
        void ClearMaterialAndMesh() override;

        void SetConfigData(AZStd::string packageName, AZStd::string componentName) override;
        void SetRenderMode(fairygui::FairyRenderMode renderMode) override;
        void SetSortingOrder(int value, bool apply) override;
        void CreateUI() override;
        fairygui::GComponent* GetComponent() override
        {
            return m_ui;
        };

        // MeshProviderRequestBus::Handler
        void UpdateMeshBuffer() override;
        bool ApplySubMesh(int index) override;

        // RendererSystemNotificationBus::Handler
        void OnPreRender() override;
 
    private:

        void CreateUI_PlayMode();
        void ReBuild();
        void DealWithChildren(fairygui::GComponent* parent);
        bool CollectData(const fairygui::Vector<fairygui::GObject*>& vec);
        bool IsFairyExtension(fairygui::GObject* obj, fairygui::FairyRenderType& type);
        void EndMask();
        void BeginMask(fairygui::GObject* pMask);

        float m_modelTM[16];

        AZStd::string m_packageName;
        AZStd::string m_componentName;
        fairygui::FairyRenderMode m_renderMode = fairygui::FairyRenderMode::ScreenSpaceOverlay;
        // AZStd::string m_renderCamera;
        int m_sortingOrder = 0;
        AZ::Vector3 m_position{0, 0, 0};
        AZ::Vector3 m_scale{1, 1, 1};
        AZ::Vector3 m_rotation{0, 0, 0};
        fairygui::FairyFitScreenMode m_fitScreen = fairygui::FairyFitScreenMode::FitSize;
        bool m_touchDisabled = false;

        fairygui::GComponent* m_ui = nullptr;
        fairygui::Container* m_container = nullptr;

        enum MaterialType
        {
            Fairy_Normal = 0,
            Font_Normal = 10
        };

        bool m_showMask = false;
        fairygui::Rect m_maskRect = fairygui::Rect::ZERO;

        AZStd::shared_ptr<Module::Material> GetMaterial(MaterialType, Module::TextureAsset*, int maskIndex);
        AZStd::shared_ptr<Module::Material> GetMaskMaterial(int maskIndex);
        AZStd::shared_ptr<Module::Material> GetShapeMaterial(int maskIndex);

        fairygui::MeshContext m_meshContext;

        AZStd::vector<fairygui::RenderQueueObject> m_vecRenderObj;
        fairygui::FairyRenderType m_currentRenderType;

        float m_panelScale = 1.0f;
        int m_maskIndex = 0;
        int m_curMaskIndex = 0;
        fairygui::BatchNode* m_renderNode = nullptr;
        fairygui::MaskNode* m_currentMaskNode = nullptr;

        bgfx::DynamicVertexBufferHandle m_vertexBufferHandle = BGFX_INVALID_HANDLE;
        bgfx::DynamicIndexBufferHandle m_indexBufferHandle = BGFX_INVALID_HANDLE;
    };
}
