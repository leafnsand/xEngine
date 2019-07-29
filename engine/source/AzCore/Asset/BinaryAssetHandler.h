#pragma once

#include <AzCore/Asset/AssetManager.h>

namespace AZ
{
    class BinaryAsset : public Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(BinaryAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(BinaryAsset, "{BC8499AB-8277-4158-A2B8-0F7310C1B7BD}", AZ::Data::AssetData);

        const void* GetBuffer() const { return m_buffer; }
        size_t GetLength() const { return m_length; }

    protected: // derived class can free or assign buffer and length
        void*  m_buffer = nullptr;
        size_t m_length = 0;

        friend class BinaryAssetHandler;
    };

    class BinaryAssetHandler : public AZ::Data::AssetHandler
    {
    public:
        AZ_CLASS_ALLOCATOR(BinaryAssetHandler, AZ::SystemAllocator, 0);
        AZ_RTTI(BinaryAssetHandler, "{A1CAEEB6-3144-489B-860E-8226EC0485B3}");

        Data::AssetPtr CreateAsset(const Data::AssetId& id, const Data::AssetType& type) override;

        bool LoadAssetData(const Data::Asset<Data::AssetData>& asset, IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB) override;

        void DestroyAsset(Data::AssetPtr ptr) override;
    };
} // namespace AZ