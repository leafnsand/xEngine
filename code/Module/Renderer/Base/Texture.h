#pragma once

#include "Renderer/Asset/TextureAsset.h"

#include <AzCore/Asset/AssetCommon.h>

#include <bgfx/bgfx.h>

namespace Module
{
    class Texture : public AZ::Data::AssetBus::Handler
    {
    public:
        AZ_CLASS_ALLOCATOR(Texture, AZ::SystemAllocator, 0);
        AZ_RTTI(Texture, "{7C4E86BF-B5D1-40DB-AFD1-4B8C8F2C13BA}");

        Texture();
        explicit Texture(const AZStd::string& path);

        // non-copyable
        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        // movable
        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

        ~Texture() override;

        static void Reflect(AZ::ReflectContext* context);

        // AZ::Data::AssetBus::Handler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        const bgfx::TextureInfo& GetInfo() const { return m_info; }

        bool IsValid() const { return bgfx::isValid(m_handle); }

        void Apply(int stage, bgfx::UniformHandle uniform);

    private:
        AZ::Data::Asset<TextureAsset> m_asset;
        bgfx::TextureHandle           m_handle = BGFX_INVALID_HANDLE;
        bgfx::TextureInfo             m_info   = {};

        friend class Sprite;
    };

    using TexturePtr = AZStd::shared_ptr<Texture>;
}
