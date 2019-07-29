#include "GImage.h"
#include "utils/ToolSet.h"
#include "display/DrawItem.h"

NS_FGUI_BEGIN

GImage::GImage()
{
    _touchDisabled = true;
    this->m_displayObject = new DrawItem("GImage",this);
}

GImage::~GImage()
{
}

void GImage::handleInit()
{
}

void GImage::handleSizeChanged()
{
    if (_packageItem && _packageItem->scaleByTile)
        this->setTextureRect(Rect(Vec2::ZERO, _size));
    else
        this->setContentSize(_size);
}

void GImage::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

//    this->setGrayed(_finalGrayed);
}

Color4B GImage::cg_getColor() const
{
    return _color4b;
}

void GImage::cg_setColor(const Color4B& value)
{
    _color4b = value;
}

void GImage::constructFromResource()
{
    sourceSize.width = _packageItem->width;
    sourceSize.height = _packageItem->height;
    initSize = sourceSize;

    this->setSpriteFrame(_packageItem->spriteFrame);
    if (_packageItem->scale9Grid)
        this->setScale9Grid(_packageItem->scale9Grid);

    setSize(sourceSize.width, sourceSize.height);
}

void GImage::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char *p;

    p = xml->Attribute("flip");
    if (p)
        setFlip(ToolSet::parseFlipType(p));

    p = xml->Attribute("color");
    if (p)
    {
//        setColor((Color3B)ToolSet::convertFromHtmlColor(p));
        cg_setColor(ToolSet::convertFromHtmlColor(p));
    }

    p = xml->Attribute("fillMethod");
    if (p)
    {
        _fillMethod = ToolSet::parseFillMethod(p);
        if(_fillMethod != FillMethod::NONE)
        {
            p = xml->Attribute("fillOrigin");
            if(p)
                _fillOrigin = atof(p);

            p = xml->Attribute("fillClockwise");
            if(p)
                _fillClockwise = strcmp(p, "true") == 0;

            p = xml->Attribute("fillAmount");
            if(p)
                _fillAmount = atof(p) / 100.0f;
        }
    }
}

void GImage::setScale9Grid(Rect* value)
{
    if (value == nullptr)
    {
//        setCenterRectNormalized(Rect(0, 0, 1, 1));
        return;
    }

    Rect insets = *value;

    // When Insets == Zero --> we should use a 1/3 of its untrimmed size
    if (insets.equals(Rect::ZERO)) {
        insets = Rect(initSize.width / 3.0f,
            initSize.height / 3.0f,
            initSize.width / 3.0f,
            initSize.height / 3.0f);
    }

    // emulate invalid insets. shouldn't be supported, but the original code supported it.
    if (insets.origin.x > initSize.width)
        insets.origin.x = 0;
    if (insets.origin.y > initSize.height)
        insets.origin.y = 0;
    if (insets.size.width > initSize.width)
        insets.size.width = 1;
    if (insets.size.height > initSize.height)
        insets.size.height = 1;

    // we have to convert from untrimmed to trimmed
    // Sprite::setCenterRect is using trimmed values (to be compatible with Cocos Creator)
    // Scale9Sprite::setCapInsects uses untrimmed values (which makes more sense)

    // use _rect coordinates. recenter origin to calculate the
    // intersecting rectangle
    // can't use _offsetPosition since it is calculated using bottom-left as origin,
    // and the center rect is calculated using top-left
//    insets.origin.x -= (initSize.width - _texRect.size.width) / 2;// +_unflippedOffsetPositionFromCenter.x;
//    insets.origin.y -= (initSize.height - _texRect.size.height) / 2;// -_unflippedOffsetPositionFromCenter.y;

    // intersecting rectangle
    const float x1 = max(insets.origin.x, 0.0f);
    const float y1 = max(insets.origin.y, 0.0f);
    const float x2 = min(insets.origin.x + insets.size.width, 0.0f + initSize.width);
    const float y2 = min(insets.origin.y + insets.size.height, 0.0f + initSize.height);

    // centerRect uses the trimmed frame origin as 0,0.
    // so, recenter inset rect
    insets.setRect(x1,
        y1,
        x2 - x1,
        y2 - y1);

    // Only update center rect while in slice mode.
    _scale9grid = insets;
//    setCenterRect(insets);    
}


NS_FGUI_END