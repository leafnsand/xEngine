#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

#include "FairyGUI/EBus/FairyMeshComponentBus.h"
#include "Renderer/EBus/RendererSystemComponentBus.h"
#include "Renderer/EBus/MeshProviderBus.h"
#include "FairyGUI/fairy/render/RenderTypes.h"

namespace fairygui
{
    class GObject;
}

namespace Module
{
    class FairyMeshComponent
        : public    AZ::Component
        , protected AZ::TransformNotificationBus::Handler
        , protected FairyMeshRequestBus::Handler
        , protected MeshProviderRequestBus::Handler
        , protected RendererSystemNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(FairyMeshComponent, "{715167E6-1D09-4CA2-957B-62965A8A4129}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("FairyMeshService"));
            provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("FairyMeshService"));
            incompatible.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("TransformService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("TransformService"));
        }

        static void Reflect(AZ::ReflectContext* reflection);

        FairyMeshComponent() {};

        // AZ::Component
        void Activate() override;
        void Deactivate() override;
    protected:
        // AZ::TransformNotificationBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
        // FairyMeshRequestBus::Handler
        void RefreshData( fairygui::NGraphics* graphics) override;
        // MeshProviderRequestBus::Handler
        void UpdateMeshBuffer() override;
        bool ApplySubMesh(int index) override;

        // RendererSystemNotificationBus::Handler
        void OnPreRender() override;

    private:
        void CaculateRenderData(fairygui::GObject* object);
        float m_modelTM[16];
        AZ::u16 m_orderInLayer = 0;
        fairygui::MeshContext m_meshContext;
        bgfx::DynamicVertexBufferHandle m_vertexBufferHandle = BGFX_INVALID_HANDLE;
        bgfx::DynamicIndexBufferHandle m_indexBufferHandle = BGFX_INVALID_HANDLE;

        AZStd::shared_ptr<Module::Material> GetImageMaterial(Module::TextureAsset* textureAsset, int stencilIndex);
        AZStd::shared_ptr<Module::Material> GetFontMaterial(Module::TextureAsset* textureAsset, int stencilIndex);
        AZStd::shared_ptr<Module::Material> GetShapeMaterial(int stencilIndex);
        AZStd::shared_ptr<Module::Material> GetMaskMaterial(int stencilIndex);

        AZStd::shared_ptr<Module::Material> m_material = nullptr;
    };
}
