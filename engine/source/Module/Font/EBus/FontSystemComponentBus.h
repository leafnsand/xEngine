#pragma once

#include <AzCore/EBus/EBus.h>
#include <AzCore/Math/Vector2.h>
#include "Renderer/Asset/TextureAsset.h"

namespace Module
{
    class FontSystemRequest : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Single;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~FontSystemRequest() {}

        virtual void LoadFont(const char* fontName, const char* fontPath) = 0;

        virtual void GetTextBounds(const char* face, const char* text, float size, float& outWidth, float& outHeight, const char* end = nullptr) = 0;
        /// start render text
        virtual void BeginText(const char* face, float x, float y, const char* text, float size) = 0;
        /// the screen and uv rectangle of the next character 
        virtual bool GetNextCharacter(AZ::Vector2& outPosMin, AZ::Vector2& outPosMax, AZ::Vector2& outUvMin, AZ::Vector2& outUvMax) = 0;
        /// finish rendering the current text strin
        virtual void EndText() = 0;

        virtual bool Decutf8(unsigned int& utf8state, unsigned int& codepoint, const char* text) = 0;

        virtual int GetFonsFontIndex(const char* face) = 0;

        virtual TextureAsset* const GetTexture(const char* face) = 0;
    };

    using FontSystemRequestBus = AZ::EBus<FontSystemRequest>;

    class FontSystemNotification : public AZ::EBusTraits
    {
    public:
        static const AZ::EBusHandlerPolicy HandlerPolicy = AZ::EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = AZ::EBusAddressPolicy::Single;

        virtual ~FontSystemNotification() {}
    };

    using FontSystemNotificationBus = AZ::EBus<FontSystemNotification>;
}
