#include "BinaryAssetHandler.h"

#include <AzCore/IO/GenericStreams.h>

namespace AZ
{
    Data::AssetPtr BinaryAssetHandler::CreateAsset(const Data::AssetId& id, const Data::AssetType& type)
    {
        return aznew BinaryAsset;
    }

    bool BinaryAssetHandler::LoadAssetData(const Data::Asset<Data::AssetData>& asset, IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB)
    {
        BinaryAsset* assetData = asset.GetAs<BinaryAsset>();
        AZ_Assert(assetData, "Asset is of the wrong type.");
        if (assetData)
        {
            assetData->m_length = stream->GetLength();
            if (assetData->m_length > 0)
            {
                assetData->m_buffer = azmalloc(assetData->m_length);
                stream->Read(assetData->m_length, assetData->m_buffer);
            }
            return true;
        }
        return false;
    }

    void BinaryAssetHandler::DestroyAsset(Data::AssetPtr ptr)
    {
        BinaryAsset* assetData = azrtti_cast<BinaryAsset*>(ptr);
        if (assetData && assetData->m_buffer)
        {
            azfree(assetData->m_buffer);
        }
        delete ptr;
    }
}
