#include "SpriteFrame.h"


NS_FGUI_BEGIN

SpriteFrame::~SpriteFrame()
{

}

SpriteFrame* SpriteFrame::createWithTexture(Texture2D* texture, const Rect& rect)
{
    SpriteFrame* spriteFrame = new SpriteFrame();
    spriteFrame->initWithTexture(texture, rect, 0, Vec2(0, 0), rect.size);
    return spriteFrame;
}

bool SpriteFrame::initWithTexture(Texture2D* pobTexture, const Rect& rect, bool rotated, const Vec2& offset, const Size& originalSize)
{
    texture = pobTexture;
    _rect = rect;
    _size = originalSize;

    if (texture) return true;

    return false;
}

void SpriteFrame::setScaleByTile(bool value)
{
    
}

void SpriteFrame::setOffset(const Vec2& offsets)
{
    _offset = offsets;
}

void SpriteFrame::setOriginalSizeInPixels(const Size& sizeInPixels)
{
    
}

void SpriteFrame::setOriginalSize(const Size& sizeInPixels)
{
    _size = sizeInPixels;
}

void SpriteFrame::setRectInPixels(const Rect& rectInPixels)
{
    _rectInpixels = rectInPixels;
}

Rect SpriteFrame::getRectInPixels()
{
    return _rectInpixels;
}

NS_FGUI_END