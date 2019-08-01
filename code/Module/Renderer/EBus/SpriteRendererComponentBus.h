#pragma once

#include "Renderer/Base/Sprite.h"

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Color.h>

namespace Module
{
    enum class SpriteDrawMode : AZ::u8
    {
        Simple = 0,
        Sliced = 1,
        Tiled  = 2,
    };

    class SpriteRendererRequest : public AZ::ComponentBus
    {
    public:
        virtual SpritePtr        GetSprite() const                = 0;
        virtual void             SetSprite(SpritePtr sprite)      = 0;

        virtual const AZ::Color& GetColor() const                 = 0;
        virtual void             SetColor(const AZ::Color& color) = 0;

        virtual SpriteDrawMode   GetDrawMode() const              = 0;
        virtual void             SetDrawMode(SpriteDrawMode mode) = 0;

        virtual bool             IsFilpX() const                  = 0;
        virtual void             SetFlipX(bool value)             = 0;

        virtual bool             IsFlipY() const                  = 0;
        virtual void             SetFlipY(bool value)             = 0;
    };

    using SpriteRendererRequestBus = AZ::EBus<SpriteRendererRequest>;
}
