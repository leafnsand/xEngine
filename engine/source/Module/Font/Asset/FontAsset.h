#pragma once

#include <AzCore/Asset/AssetManager.h>

namespace Module
{
    class FontAsset : public AZ::Data::AssetData
    {
    public:
        AZ_CLASS_ALLOCATOR(FontAsset, AZ::SystemAllocator, 0);
        AZ_RTTI(FontAsset, "{E7CB96B4-DF8B-4AFA-B97B-A2F9252FCBB1}", AZ::Data::AssetData);

        ~FontAsset() override;

        void* GetData() const { return m_data; }

        size_t GetLength() const { return m_length; }

        void SetFontName(const char* name) { m_fontName = name; }
        AZStd::string GetFontName() const { return m_fontName; }
    private:
        void* m_data = nullptr;
        size_t m_length = 0;

        AZStd::string m_fontName = "";

        friend class FontAssetHandler;
    };
}