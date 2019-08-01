#pragma once

#include "FairyGUIMacros.h"

#include "base/Base.h"
#include "display/Texture2D.h"

NS_FGUI_BEGIN

class SpriteFrame
{
public:
    ~SpriteFrame();

    static SpriteFrame* createWithTexture(Texture2D* texture, const Rect& rect);

    bool initWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize);

    void setScaleByTile(bool value);

    void setOffset(const Vec2& offsets);

    void setOriginalSizeInPixels(const Size& sizeInPixels);

    void setOriginalSize(const Size& sizeInPixels);

    void setRectInPixels(const Rect& rectInPixels);

    Rect getRectInPixels();

    Texture2D* texture = nullptr;

    Rect _rect = Rect::ZERO;
    Size _size = Size::ZERO;
    Vec2 _offset = Vec2::ZERO;
private:
    Rect _rectInpixels = Rect::ZERO;
};

NS_FGUI_END
