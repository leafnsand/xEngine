#include "Font/Asset/FontAssetHandler.h"
#include "Font/Asset/FontAsset.h"

#include <AzCore/IO/GenericStreams.h>

namespace Module
{
    bool FontAssetHandler::OnLoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, void* buffer, size_t length)
    {
        FontAsset* assetData = asset.GetAs<FontAsset>();
        assetData->m_length = length;
        void* data = azmalloc(length);
        memcpy(data, buffer, length);
        assetData->m_data = data;
        return true;
    }
}
