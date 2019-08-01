#pragma once

#include "AzCore/Component/Component.h"
#include "AzCore/Component/TransformBus.h"
#include "AzCore/Component/TickBus.h"
#include "Renderer/EBus/MeshProviderBus.h"
#include "Particle2d/EBus/Particle2dComponentBus.h"
#include "AzCore/Asset/AssetManager.h"

#include "Particle2d/Asset/Particle2dDescAsset.h"
#include "Particle2d/Priv/Particle2dNode.h"
#include "Particle2d/Priv/Particle2dItem.h"

namespace Particle2d 
{
    // class Particle2DItem;
    class Particle2DLauncher;
    class Particle2DUnit;
}

namespace Module
{
    class Particle2dComponent
        : public AZ::Component
        , protected AZ::TransformNotificationBus::Handler
        , protected AZ::TickBus::Handler
        , protected MeshProviderRequestBus::Handler
        , protected AZ::Data::AssetBus::MultiHandler
        , protected Particle2dRequestBus::Handler
        , protected Module::RendererSystemNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(Particle2dComponent, "{A0777DC8-E10B-4C50-919F-FA389EF97A23}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("Particle2dService"));
            provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("Particle2dService"));
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
        
        Particle2dComponent(){};
        Particle2dComponent(const AZStd::string& descPath);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

    protected:
        // AZ::TransformNotificationBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
        // AZ::TickBus::Handler
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        // MeshProviderRequestBus::Handler
        void UpdateMeshBuffer();
        bool ApplySubMesh(int index);
        // AZ::Data::AssetBus::MultiHandler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
        // Module::RendererSystemNotificationBus::Handler
        void OnPreRender() override;
        // Particle2dRequestBus::Handler
        bool IsValid() const override;
        void Stop() override;
        void Restart() override;
        void Pause() override;
        void Resume() override;
        void Clean() override;
    private:

        void CleanBuffer();
        void Discard();

        bool m_isValue = false;
        bool m_isRunning = true;
        bool m_hasClean = false;
        float m_liveTime = 0;
        
        AZStd::string m_descPath;
        AZ::Data::Asset<Particle2dDescAsset> m_descAsset;

        Particle2d::Particle2DItem m_rootItem;

        float m_modelTM[16];

        AZStd::vector<Particle2d::Particle2dNode*> m_nodeVector;

        AZStd::vector<int> m_childNodeVector;

 

        bgfx::DynamicVertexBufferHandle m_vertexBufferHandle = BGFX_INVALID_HANDLE;
        bgfx::DynamicIndexBufferHandle m_indexBufferHandle = BGFX_INVALID_HANDLE;

        int m_vertexCount = 0, m_indexCount = 0;
        AZStd::vector<Particle2d::VertexData> m_vertexBuffer;
        AZStd::vector<uint16_t> m_indexBuffer;
        
        friend class Particle2d::Particle2DItem;
        friend class Particle2d::Particle2DLauncher;
        friend class Particle2d::Particle2DUnit;
        friend class Particle2d::Particle2dNode;
    };
}
