#pragma once

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <AzCore/std/smart_ptr/unique_ptr.h>

#include "Renderer/EBus/RendererSystemComponentBus.h"
#include "Window/EBus/WindowSystemComponentBus.h"

namespace Module
{
    class RendererSystemComponent
        : public AZ::Component
        , protected AZ::SystemTickBus::Handler
        , protected RendererSystemRequestBus::Handler
        , protected WindowsSystemNotificationBus::Handler
    {
    public:
        AZ_COMPONENT(RendererSystemComponent, "{89CDA8FE-66DE-44AA-ADD6-B5FDA8FB1E11}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("RendererSystemService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("RendererSystemService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("WindowSystemService"));
            dependent.push_back(AZ_CRC("AssetDatabaseService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("WindowSystemService"));
            required.push_back(AZ_CRC("AssetDatabaseService"));
        }

        static void Reflect(AZ::ReflectContext* context);

        RendererSystemComponent();
        RendererSystemComponent(const RendererSystemComponent&) = delete;
        ~RendererSystemComponent() override = default;

    protected:
        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::SystemTickBus::Handler
        void OnSystemTick() override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // RendererSystemRequestBus::Handler
        void SetResetFlags(uint32_t resetFlags) override;

        TexturePtr  GetTexture(const AZStd::string& path) override;
        ShaderPtr   GetShader(const AZStd::string& path) override;
        MaterialPtr GetMaterial(const AZStd::string& path) override;
        MeshPtr     GetMesh(AZStd::string& path) override;
        SpritePtr   GetSprite(const AZStd::string& path, const AZStd::string& spriteName) override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        //WindowsSystemNotificationBus::Handler
        void OnWindowSizeChanged(int width, int height) override;
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>> m_assetHandlers;

        AZStd::unordered_map<AZStd::string, AZStd::weak_ptr<Texture>>  m_textures;
        AZStd::unordered_map<AZStd::string, AZStd::weak_ptr<Shader>>   m_shaders;
        AZStd::unordered_map<AZStd::string, AZStd::weak_ptr<Material>> m_materials;
        AZStd::unordered_map<AZStd::string, AZStd::weak_ptr<Mesh>>     m_meshes;
        AZStd::unordered_map<AZStd::string, AZStd::weak_ptr<Sprite>>   m_sprites;

        uint32_t m_resetFlags = BGFX_RESET_NONE;
    };
}
