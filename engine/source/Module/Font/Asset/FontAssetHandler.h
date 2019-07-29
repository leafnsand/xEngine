#pragma once

#include "FontAsset.h"

#include <AzCore/Asset/GenericAssetHandler.h>

namespace Module
{
    class FontAssetHandler : public AZ::GenericAssetHandler<FontAsset>
    {
    public:
        AZ_CLASS_ALLOCATOR(FontAssetHandler, AZ::SystemAllocator, 0);

        bool OnLoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, void* buffer, size_t length) override;
    };
}
