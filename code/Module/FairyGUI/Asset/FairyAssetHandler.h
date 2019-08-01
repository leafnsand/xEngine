#pragma once

#include "FairyAsset.h"

#include <AzCore/Asset/GenericAssetHandler.h>

namespace Module
{
    class FairyAssetHandler : public AZ::GenericAssetHandler<FairyAsset>
    {
    public:
        AZ_CLASS_ALLOCATOR(FairyAssetHandler, AZ::SystemAllocator, 0);

        bool OnLoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, void* buffer, size_t length) override;
    };
}
