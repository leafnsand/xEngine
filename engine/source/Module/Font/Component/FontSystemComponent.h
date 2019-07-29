#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>

#include <Font/EBus/FontSystemComponentBus.h>
#include <Font/Asset/FontAssetHandler.h>
#include "priv/fontstash.h"
#include "AzCore/std/smart_ptr/unique_ptr.h"

namespace Module
{
    class FontSystemComponent
        : public AZ::Component
        , protected FontSystemRequestBus::Handler
        , protected AZ::SystemTickBus::Handler
        , protected AZ::Data::AssetBus::MultiHandler
    {
    public:
        AZ_COMPONENT(FontSystemComponent, "{8920F112-7ADA-4492-9763-B3F6E2856650}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("FontService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("FontService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("RendererSystemService"));
            dependent.push_back(AZ_CRC("AssetDatabaseService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("RendererSystemService"));
            required.push_back(AZ_CRC("AssetDatabaseService"));
        }

        static void Reflect(AZ::ReflectContext* reflection);

        FontSystemComponent();
        FontSystemComponent(const FontSystemComponent&) = delete;
        ~FontSystemComponent() override = default;

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        // AZ::SystemTickBus::Handler
        void OnSystemTick() override;

        // AZ::Data::AssetBus::MultiHandler
        void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

        // FontSystemRequestBus::Handler
        void LoadFont(const char* fontName, const char* fontPath) override;
        void GetTextBounds(const char* face, const char* text, float size, float& outWidth, float& outHeight, const char* end = nullptr) override;
        void BeginText(const char* face, float x, float y, const char* text, float size) override;
        bool GetNextCharacter(AZ::Vector2& outPosMin, AZ::Vector2& outPosMax, AZ::Vector2& outUvMin, AZ::Vector2& outUvMax) override;
        void EndText() override;
        bool Decutf8(unsigned int& utf8state, unsigned int& codepoint, const char* text) override;
        int GetFonsFontIndex(const char* face) override;
        TextureAsset* const GetTexture(const char* face) override;
    private:
        void UpdateTexture();

        AZStd::unordered_map<AZStd::string, int> m_fonsFontMap;
        int m_defaultFont = -1;
        FONScontext* m_fonsContext = nullptr;
        FONStextIter m_fonsIterator;
        AZStd::string m_defaultFontName = "";

        AZStd::vector<AZStd::unique_ptr<AZ::Data::AssetHandler>> m_assetHandlers;

        TextureAsset* m_texAsset = nullptr;
        AZStd::vector<FontAsset*> m_vecFontAssets;
    };
}
