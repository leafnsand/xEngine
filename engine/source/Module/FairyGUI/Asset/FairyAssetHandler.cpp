#include "FairyGUI/Asset/FairyAssetHandler.h"
#include "FairyGUI/Asset/FairyAsset.h"

#include <AzCore/IO/GenericStreams.h>

namespace Module
{
    bool FairyAssetHandler::OnLoadAssetData(const AZ::Data::Asset<AZ::Data::AssetData>& asset, void* buffer, size_t length)
    {
        FairyAsset* assetData = asset.GetAs<FairyAsset>();
        assetData->m_length = length;
        void* data = azmalloc(length);
        memcpy(data, buffer, length);
        assetData->m_data = data;
        return true;
    }
}
