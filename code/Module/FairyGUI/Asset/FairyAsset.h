#pragma once

#include <AzCore/Asset/AssetManager.h>

struct spAtlas;

namespace Module
{
    class FairyAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(FairyAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(FairyAsset, "{A8988908-443C-4BCA-A908-9CF3782814F6}", AZ::Data::AssetData);

        ~FairyAsset() override;

        void* GetData() const { return m_data; }

        size_t GetLength() const { return m_length; }

    private:
        void* m_data = nullptr;
        size_t m_length = 0;

        friend class FairyAssetHandler;
    };


}