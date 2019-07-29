#pragma once

#include "Renderer/Asset/MaterialAsset.h"
#include "Renderer/Base/Texture.h"
#include "Renderer/Base/Shader.h"

#include <AzCore/std/smart_ptr/shared_ptr.h>

#include <AzCore/Asset/AssetCommon.h>

#include <bgfx/bgfx.h>

namespace Module
{
    class Material : public AZ::Data::AssetBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(Material, AZ::SystemAllocator, 0);
        AZ_RTTI(Material, "{3E966CA9-ABD3-4278-85EB-6DF1082A2B4A}");

        explicit Material(const AZStd::string& relativePath);

        // non-copyable
        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        static void Reflect(AZ::ReflectContext* context);

        // AZ::Data::AssetBus::Handler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        void SetVector(const AZStd::string& name, const AZ::Vector4& vector);
        void SetTexture(const AZStd::string& name, TexturePtr texture);

        bool IsValid() const;

        void SetReverseCull(bool value);

        void Apply();

        AZ::s32 GetQueue() const { return m_queue; }
        void SetQueue(AZ::s32 queue);

    private:
        struct Property
        {
            AZ_CLASS_ALLOCATOR(Property, AZ::SystemAllocator, 0);

            enum class Type : AZ::u8
            {
                Vector,
                Texture,
            };

            Property(const MaterialAsset::Property& config);
            ~Property();

            // non-copyable
            Property(const Property&) = delete;
            Property& operator=(const Property&) = delete;

            Property(Property&&) noexcept;
            Property& operator=(Property&&) noexcept;

            void Apply(int textureStage);

            bgfx::UniformHandle           m_uniform        = BGFX_INVALID_HANDLE;
            AZStd::string                 m_name;
            Type                          m_type           = Type::Vector;
            float                         m_vectorValue[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // use raw array here to reduce convertion
            TexturePtr                    m_textureValue;
        };

        AZ::Data::Asset<MaterialAsset> m_config;

        AZ::s32                        m_queue = 0;
        ShaderPtr                      m_shader;

        AZStd::vector<Property>        m_properties;

        friend class RendererSystemComponent;
    };

    using MaterialPtr = AZStd::shared_ptr<Material>;
}
