#pragma once

#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector3.h>
#include <AzCore/Math/Vector4.h>

#include <bgfx/bgfx.h>

namespace AZ {
    class ReflectContext;
}

namespace Module
{
    enum class SpritePivot : AZ::u8
    {
        Center      = 0,
        TopLeft     = 1,
        Top         = 2,
        TopRight    = 3,
        Left        = 4,
        Right       = 5,
        BottomLeft  = 6,
        Bottom      = 7,
        BottomRight = 8,
        Custom      = 9,
    };

    enum class TextureWrapMode : AZ::u8
    {
        Repeat     = 0,
        Clamp      = 1,
        Mirror     = 2,
        MirrorOnce = 3,
    };

    enum class TextureFilterMode : AZ::u8
    {
        Point     = 0,
        Bilinear  = 1,
        Trilinear = 2,
    };

    enum class SpriteMeshType : AZ::u8
    {
        FullRect = 0,
        Tight    = 1,
    };

    class TextureAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(TextureAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(TextureAsset, "{845BA769-9F02-4F88-8DF1-739A705BA62F}", AZ::Data::AssetData);

        static void Reflect(AZ::ReflectContext* context);

        struct SpriteData
        {
            AZ_CLASS_ALLOCATOR(SpriteData, AZ::SystemAllocator, 0);

            AZStd::string              m_name;
            AZStd::vector<AZ::Vector3> m_positions;
            AZStd::vector<AZ::Vector2> m_texcoords;
            AZStd::vector<AZ::u16>     m_indices;
            AZ::Vector4                m_border;
            AZ::Vector4                m_size;
            AZ::Vector2                m_pivot;
            SpritePivot                m_pivotType = SpritePivot::Custom;
        };

    private:
        AZStd::vector<SpriteData> m_sprites;
        float                     m_pixelsToUnits = 100.0f;
        TextureWrapMode           m_uWrapMode     = TextureWrapMode::Repeat;
        TextureWrapMode           m_vWrapMode     = TextureWrapMode::Repeat;
        TextureFilterMode         m_filterMode    = TextureFilterMode::Bilinear;
        SpriteMeshType            m_meshType      = SpriteMeshType::FullRect;

        bgfx::TextureHandle m_handle = BGFX_INVALID_HANDLE;
        bgfx::TextureInfo   m_info   = {};

        friend class TextureAssetHandler;
        friend class Texture;
        friend class Sprite;
    };
}
