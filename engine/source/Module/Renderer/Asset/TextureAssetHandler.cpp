#include "Renderer/Asset/TextureAssetHandler.h"

#include <AzCore/IO/GenericStreams.h>

#include <bimg/bimg.h>

namespace bgfx
{
    extern bx::AllocatorI* g_allocator;
}

namespace Module
{
    AZ::Data::AssetPtr TextureAssetHandler::CreateAsset(const AZ::Data::AssetId& id, const AZ::Data::AssetType& type)
    {
        return aznew TextureAsset;
    }

    bool TextureAssetHandler::LoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB)
    {
        TextureAsset* assetData = asset.GetAs<TextureAsset>();

        const size_t length = stream->GetLength();
        void* buffer = azmalloc(length);
        stream->Read(length, buffer);

        uint32_t flags = 0;

        bgfx::TextureHandle handle = BGFX_INVALID_HANDLE;

        auto textureAssetBuffer = static_cast<char*>(buffer);
        textureAssetBuffer = textureAssetBuffer + length - sizeof(int);
        const int texLength = *reinterpret_cast<int*>(textureAssetBuffer);
        if (texLength != 0)
        {
            textureAssetBuffer = (char*)buffer + texLength;
            const auto textureAsset = AZ::Utils::LoadObjectFromBuffer<TextureAsset>(textureAssetBuffer, length - 4 - texLength);

            switch (textureAsset->m_uWrapMode)
            {
            case TextureWrapMode::Clamp:
                flags |= BGFX_TEXTURE_U_CLAMP;
                break;
            case TextureWrapMode::Mirror:
                flags |= BGFX_TEXTURE_U_MIRROR;
                break;
            case TextureWrapMode::MirrorOnce:
                flags |= BGFX_TEXTURE_U_MIRROR;
                break;
            case TextureWrapMode::Repeat:
                break;
            }

            switch (textureAsset->m_vWrapMode)
            {
            case TextureWrapMode::Clamp:
                flags |= BGFX_TEXTURE_V_CLAMP;
                break;
            case TextureWrapMode::Mirror:
                flags |= BGFX_TEXTURE_V_MIRROR;
                break;
            case TextureWrapMode::MirrorOnce:
                flags |= BGFX_TEXTURE_V_MIRROR;
                break;
            case TextureWrapMode::Repeat:
                break;
            }

            switch (textureAsset->m_filterMode)
            {
            case TextureFilterMode::Point:
                flags |= BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIP_POINT;
                break;
            case TextureFilterMode::Bilinear:
                break;
            case TextureFilterMode::Trilinear:
                flags |= BGFX_TEXTURE_MIN_ANISOTROPIC | BGFX_TEXTURE_MAG_ANISOTROPIC;
                break;
            }
        }

        const auto caps = bgfx::getCaps();

        bimg::ImageContainer* imageContainer = nullptr;
        if (caps->formats[bgfx::TextureFormat::BC3] != BGFX_CAPS_FORMAT_TEXTURE_NONE)
        {
            imageContainer = bimg::imageParseDds(bgfx::g_allocator, buffer, length, nullptr);
        }
        else if (caps->formats[bgfx::TextureFormat::PTC14] != BGFX_CAPS_FORMAT_TEXTURE_NONE)
        {
            imageContainer = bimg::imageParsePvr3(bgfx::g_allocator, buffer, length, nullptr);
        }
        else if (caps->formats[bgfx::TextureFormat::ETC1] != BGFX_CAPS_FORMAT_TEXTURE_NONE)
        {
            imageContainer = bimg::imageParseKtx(bgfx::g_allocator, buffer, length, nullptr);
        }

        if (imageContainer)
        {
            if (imageContainer->m_cubeMap)
            {
                handle = bgfx::createTextureCube(
                    uint16_t(imageContainer->m_width)
                    , 1 < imageContainer->m_numMips
                    , imageContainer->m_numLayers
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , flags
                    , bgfx::copy(imageContainer->m_data, imageContainer->m_size)
                );
            }
            else if (1 < imageContainer->m_depth)
            {
                handle = bgfx::createTexture3D(
                    uint16_t(imageContainer->m_width)
                    , uint16_t(imageContainer->m_height)
                    , uint16_t(imageContainer->m_depth)
                    , 1 < imageContainer->m_numMips
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , flags
                    , bgfx::copy(imageContainer->m_data, imageContainer->m_size)
                );
            }
            else
            {
                handle = bgfx::createTexture2D(
                    uint16_t(imageContainer->m_width)
                    , uint16_t(imageContainer->m_height)
                    , 1 < imageContainer->m_numMips
                    , imageContainer->m_numLayers
                    , bgfx::TextureFormat::Enum(imageContainer->m_format)
                    , flags
                    , bgfx::copy(imageContainer->m_data, imageContainer->m_size)
                );
            }

            bgfx::setName(handle, asset.GetHint().c_str());

            bgfx::calcTextureSize(
                assetData->m_info
                , uint16_t(imageContainer->m_width)
                , uint16_t(imageContainer->m_height)
                , uint16_t(imageContainer->m_depth)
                , imageContainer->m_cubeMap
                , 1 < imageContainer->m_numMips
                , imageContainer->m_numLayers
                , bgfx::TextureFormat::Enum(imageContainer->m_format)
            );

            bimg::imageFree(imageContainer);
        }

        azfree(buffer);

        assetData->m_handle = handle;

        return bgfx::isValid(assetData->m_handle);
    }

    void TextureAssetHandler::DestroyAsset(AZ::Data::AssetPtr ptr)
    {
        auto* textureAsset = azrtti_cast<TextureAsset*>(ptr);
        if (textureAsset)
        {
            if (bgfx::isValid(textureAsset->m_handle))
            {
                bgfx::destroy(textureAsset->m_handle);
            }
        }
        delete ptr;
    }
}
