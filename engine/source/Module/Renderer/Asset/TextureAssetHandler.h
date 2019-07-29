#pragma once

#include <Renderer/Asset/TextureAsset.h>

#include <AzCore/Asset/GenericAssetHandler.h>

namespace Module
{
    class TextureAssetHandler : public AZ::AutoRegisterAssetHandler<TextureAsset>
    {
    public:
        AZ_CLASS_ALLOCATOR(TextureAssetHandler, AZ::SystemAllocator, 0);

        AZ::Data::AssetPtr CreateAsset(const AZ::Data::AssetId& id, const AZ::Data::AssetType& type) override;

        bool LoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, AZ::IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB) override;

        void DestroyAsset(AZ::Data::AssetPtr ptr) override;
    };
}