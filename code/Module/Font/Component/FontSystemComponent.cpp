#include <Font/Component/FontSystemComponent.h>
#include "fonttypes.h"
#include "Asset/FontAsset.h"
#include "AzCore/PlatformIncl.h"

#if defined(AZ_PLATFORM_WINDOWS)
#   pragma warning(push)
#   pragma warning(disable:4267)
#elif defined(AZ_PLATFORM_EMSCRIPTEN)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-function"
#endif

#define FONS_VERTEX_COUNT 1
#define FONS_MAX_STATES 1

#define FONTSTASH_IMPLEMENTATION
#include "priv/fontstash.h"

#if defined(AZ_PLATFORM_WINDOWS)
#   pragma warning(pop)
#elif defined(AZ_PLATFORM_EMSCRIPTEN)
#   pragma clang diagnostic pop
#endif

#include "Renderer/Asset/TextureAsset.h"
#include "Renderer/EBus/RendererSystemComponentBus.h"

#include <AzCore/Serialization/SerializeContext.h>
#include "AzCore/RTTI/BehaviorContext.h"

namespace Module
{
    void FontSystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        /*
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<FontSystemComponent, AZ::Component>()
                ;
        }
        */
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<FontSystemComponent, AZ::Component>()
                ->SerializerForEmptyClass()
                ;
        }

        if (AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->EBus<FontSystemRequestBus>("FontSystemRequestBus")
                //->Event("CreateGameEntity", &GameEntityContextRequestBus::Events::CreateGameEntityForBehaviorContext)
                //->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)
                ->Event("LoadFont", &FontSystemRequestBus::Events::LoadFont)
                ;
        }
    }

    FontSystemComponent::FontSystemComponent()
    {
        m_assetHandlers.emplace_back(aznew FontAssetHandler);
    }

    void FontSystemComponent::Init()
    {
    }

    void FontSystemComponent::Activate()
    {
        N3Font::FontSetup setup;
        FONSparams params;
        memset(&params, 0, sizeof(params));

        params.userPtr = this;
        params.width = setup.TextureWidth;
        params.height = setup.TextureHeight;
        switch (setup.Origin)
        {
        case N3Font::OriginType::OriginTopLeft:
            params.flags = FONS_ZERO_TOPLEFT;
            break;
        default:
            params.flags = FONS_ZERO_BOTTOMLEFT;
            break;
        }
        this->m_fonsContext = fonsCreateInternal(&params);
        
        // create texture
        AZ::Data::Asset<Module::TextureAsset> asset;
        EBUS_EVENT_RESULT(asset, Module::RendererSystemRequestBus, CreateTexture2D, "test", params.width, params.height, bgfx::TextureFormat::Enum::R8, 0, nullptr);
        this->m_texAsset = asset.Get(); 

        FontSystemRequestBus::Handler::BusConnect();
        AZ::SystemTickBus::Handler::BusConnect();
    }

    void FontSystemComponent::Deactivate()
    {
        fonsDeleteInternal(this->m_fonsContext);
        this->m_fonsContext = nullptr;

        if(m_vecFontAssets.size() > 0)
        {
            for(auto& it : m_vecFontAssets)
            {
                it->Release();
            }
        }
        m_vecFontAssets.clear();

        AZ::Data::Asset<Module::TextureAsset> asset = m_texAsset;
        EBUS_EVENT(Module::RendererSystemRequestBus, ReleaseTexture, asset);

        AZ::SystemTickBus::Handler::BusDisconnect();
        FontSystemRequestBus::Handler::BusDisconnect();
    }

    void FontSystemComponent::OnSystemTick()
    {
        this->UpdateTexture();
    }
    
    void FontSystemComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {        
        FontAsset* fa = asset.GetAs<FontAsset>();
        fa->Acquire();
        m_vecFontAssets.push_back(fa);

        FONSsdfSettings settings;
        settings.sdfEnabled = 1;
        settings.onedgeValue = 127;
        settings.padding = 1;
        settings.pixelDistScale = 62.0f;
        settings.fontSize = 20;
        int fonsFont = fonsAddFontSdfMem(this->m_fonsContext, fa->GetFontName().c_str(), (unsigned char*)fa->GetData(), fa->GetLength(), 0, settings);

        this->m_fonsFontMap[fa->GetFontName()] = fonsFont;
        if (this->m_defaultFont == -1 && this->m_defaultFontName == fa->GetFontName())
            this->m_defaultFont = fonsFont;

        AZ_Printf("Font", "FontFacade: loaded TTF file '%s'\n", fa->GetFontName().c_str());

        AZ::Data::AssetBus::MultiHandler::BusDisconnect(asset.GetId());
    }

    void FontSystemComponent::LoadFont(const char* fontName, const char* fontPath)
    {
        if (this->m_defaultFontName.empty())
            this->m_defaultFontName = fontName;

        AZ::Data::Asset<FontAsset> fontAsset;
        fontAsset.Create(fontPath);
        fontAsset.Get()->SetFontName(fontName);

        if (fontAsset.GetId().IsValid())
        {
            AZ::Data::AssetBus::MultiHandler::BusConnect(fontAsset.GetId());
            fontAsset.QueueLoad();
        }
    }

    void FontSystemComponent::GetTextBounds(const char* face, const char* text, float size, float& outWidth, float& outHeight, const char* end)
    {
        int fonsFont = this->GetFonsFontIndex(face);

        static float bounds[4];
        fonsSetFont(this->m_fonsContext, fonsFont);
        fonsSetSize(this->m_fonsContext, size);
        fonsSetAlign(this->m_fonsContext, FONS_ALIGN_BOTTOM | FONS_ALIGN_LEFT);
        fonsTextBounds(this->m_fonsContext, 0, 0, text, end, bounds);
        outWidth = bounds[2] - bounds[0];
        outHeight = bounds[3] - bounds[1];
    }

    void FontSystemComponent::BeginText(const char* face, float x, float y, const char* text, float size)
    {
        int fonsFont = this->GetFonsFontIndex(face);

        fonsSetFont(this->m_fonsContext, fonsFont);
        fonsSetSize(this->m_fonsContext, size);
        fonsSetAlign(this->m_fonsContext, FONS_ALIGN_BOTTOM | FONS_ALIGN_LEFT);
        fonsTextIterInit(this->m_fonsContext, &this->m_fonsIterator, x, y, text, nullptr);
    }

    bool FontSystemComponent::GetNextCharacter(AZ::Vector2& outPosMin, AZ::Vector2& outPosMax, AZ::Vector2& outUvMin, AZ::Vector2& outUvMax)
    {
        static FONSquad quad;
        if (fonsTextIterNext(this->m_fonsContext, &this->m_fonsIterator, &quad))
        {
            outPosMin.Set(quad.x0, quad.y0);
            outPosMax.Set(quad.x1, quad.y1);
            outUvMin.Set(quad.s0, quad.t0);
            outUvMax.Set(quad.s1, quad.t1);
            return true;
        }
        return false;
    }

    void FontSystemComponent::EndText()
    {
        
    }

    bool FontSystemComponent::Decutf8(unsigned int& utf8state, unsigned int& codepoint, const char* text)
    {
        return fons__decutf8(&utf8state, &codepoint, *(const unsigned char*)text) != 0;
    }

    TextureAsset* const FontSystemComponent::GetTexture(const char* face)
    {
        return this->m_texAsset;
    }

    void FontSystemComponent::UpdateTexture()
    {
        if ((this->m_fonsFontMap.size() > 0) && (this->m_fonsContext))
        {
            int dirty[4];
            if (fonsValidateTexture(this->m_fonsContext, dirty))
            {
                int width, height;
                const auto buffer = fonsGetTextureData(this->m_fonsContext, &width, &height);
                bgfx::updateTexture2D(this->m_texAsset->GetHandle(), 0, 0, 0, 0, width, height, bgfx::copy(buffer, width * height));
            }
        }
    }

    int FontSystemComponent::GetFonsFontIndex(const char* face)
    {
        int fonsFont = this->m_defaultFont;
        if (this->m_fonsFontMap.size() > 0)
        {
            if (face != nullptr)
            {
                auto it = this->m_fonsFontMap.find(face);
                if (it != this->m_fonsFontMap.end())
                    fonsFont = it->second;
            }
        }
        return fonsFont;
    }
}
