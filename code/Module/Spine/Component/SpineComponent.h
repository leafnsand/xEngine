#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Asset/BinaryAssetHandler.h>

#include <bgfx/bgfx.h>

#include "Spine/EBus/SpineComponentBus.h"
#include "Renderer/EBus/MeshProviderBus.h"

struct spAtlas;
struct spSkeleton;
struct spAnimationState;

namespace Module
{
    class TextureAsset;

    class SpineComponent
        : public AZ::Component
        , protected AZ::TickBus::Handler
        , protected AZ::TransformNotificationBus::Handler
        , protected AZ::Data::AssetBus::MultiHandler
        , protected SpineRequestBus::Handler
        , protected MeshProviderRequestBus::Handler
    {
    public:
        AZ_COMPONENT(SpineComponent, "{6A2CDA4A-674A-4A4C-82C8-40ED27B5806E}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("SpineService"));
            provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("SpineService"));
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

        SpineComponent();
        SpineComponent(const AZStd::string& name);

        // AZ::Component
        void Activate() override;
        void Deactivate() override;

    protected:
        // AZ::TickBus::Handler
        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;
        // AZ::TransformBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world);
        // AZ::Data::AssetBus::Handler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
        // SpineRequestBus::Handler
        bool IsValid() const override;
        void SetSkin(const AZStd::string& skinName) override;
        void SetMix(const AZStd::string& fromAnim, const AZStd::string& toAnim, float duration) override;
        void ClearTracks() override;
        void ClearTrack(int trackIndex) override;
        void SetAnimation(int trackIndex, const AZStd::string& animationName, bool loop) override;
        void AddAnimation(int trackIndex, const AZStd::string& animationName, bool loop, float delay) override;
        void SetEmptyAnimation(int trackIndex, float mixDuration) override;
        void AddEmptyAnimation(int trackIndex, float mixDuration, float delay) override;
        void SetEmptyAnimations(float mixDuration) override;
        // MeshProviderRequestBus::Handler
        void UpdateMeshBuffer() override;
        bool ApplySubMesh(int index) override;

    private:
        AZStd::string m_name;

        AZ::Data::Asset<AZ::BinaryAsset> m_atlasAsset;
        AZ::Data::Asset<AZ::BinaryAsset> m_skeletonAsset;

        spAtlas* m_atlas = nullptr;
        spSkeleton* m_skeleton = nullptr;
        spAnimationState* m_state = nullptr;

        struct Context
        {
            int BlendMode;
            TextureAsset* Texture = nullptr;
            int StartIndex, Count;
        };

        AZStd::vector<Context> m_contextQueue;

        bgfx::DynamicVertexBufferHandle m_vertexBufferHandle = BGFX_INVALID_HANDLE;
        bgfx::DynamicIndexBufferHandle m_indexBufferHandle = BGFX_INVALID_HANDLE;

        struct Vertex {
            float x, y, u, v;
            float r, g, b, a;
        };

        AZStd::vector<Vertex> m_vertexBuffer;
        AZStd::vector<AZ::u16> m_indexBuffer;

        float m_modelTM[16];
        AZStd::shared_ptr<Mesh> m_mesh;
    };
}