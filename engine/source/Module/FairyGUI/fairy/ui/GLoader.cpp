#include "GLoader.h"
#include "UIPackage.h"
#include "utils/ToolSet.h"
#include "display/SpriteFrame.h"
#include "display/Animation.h"

NS_FGUI_BEGIN


GLoader::GLoader() :
    _autoSize(false),
    _align(TextHAlignment::LEFT),
    _verticalAlign(TextVAlignment::TOP),
    _fill(LoaderFillType::NONE),
    _updatingLayout(false),
    _contentItem(nullptr),
    _contentStatus(0),
    _spriteFrame(nullptr),
    _playing(true),
    _updatetime(0),
    _curFrame(0),
    _interval(0),
    _frameCount(0)
{
}

GLoader::~GLoader()
{
//    FUI_SAFE_RELEASE(_playAction);
//    FUI_SAFE_RELEASE(_content);
    this->clearContent();
}

void GLoader::handleInit()
{
//    _content = FUISprite::create();
//    _content->retain();
    this->setAnchorPoint(Vec2::ZERO);
    this->setCascadeOpacityEnabled(true);

//    _displayObject = Node::create();
//    _displayObject->retain();
//    _displayObject->addChild(_content);
}

void GLoader::setURL(const AZStd::string & value)
{
    if (_url.compare(value) == 0)
        return;

    _url = value;
    loadContent();
    updateGear(7);
}

void GLoader::setAlign(TextHAlignment value)
{
    if (_align != value)
    {
        _align = value;
        updateLayout();
    }
}

void GLoader::setVerticalAlign(TextVAlignment value)
{
    if (_verticalAlign != value)
    {
        _verticalAlign = value;
        updateLayout();
    }
}

void GLoader::setAutoSize(bool value)
{
    if (_autoSize != value)
    {
        _autoSize = value;
        updateLayout();
    }
}

void GLoader::setFill(LoaderFillType value)
{
    if (_fill != value)
    {
        _fill = value;
        updateLayout();
    }
}

void GLoader::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
//        if (_playAction)
//        {
//            if (_playing)
//                _content->runAction(_playAction);
//            else
//                _content->stopAction(_playAction);
//        }
        updateGear(5);
    }
}

void GLoader::setCurrentFrame(int value)
{
    _curFrame = value;
//    if (_playAction)
//        _playAction->setCurrentFrame(value);
    updateGear(5);
}

Color4B GLoader::cg_getColor() const
{
//    return (Color4B)_content->getColor();
    return _color4b;
}

void GLoader::cg_setColor(const Color4B& value)
{
//    _content->setColor((Color3B)value);
    _color4b = value;
}

void GLoader::loadContent()
{
    clearContent();

    if (_url.length() == 0)
        return;

    if (_url.compare(0, 5, "ui://") == 0)
        loadFromPackage();
    else
    {
        _contentStatus = 3;
        loadExternal();
    }
}

void GLoader::loadFromPackage()
{
    _contentItem = UIPackage::getItemByURL(_url);

    if (_contentItem != nullptr)
    {
        _contentItem->load();

        if (_contentItem->type == PackageItemType::IMAGE)
        {
            _contentStatus = 1;
            _contentSourceSize.width = _contentItem->width;
            _contentSourceSize.height = _contentItem->height;
            this->_spriteFrame = _contentItem->spriteFrame;
//            _content->initWithSpriteFrame(_contentItem->spriteFrame);
            if (_contentItem->scale9Grid)
                this->setScale9Grid(_contentItem->scale9Grid);
            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::MOVIECLIP)
        {
            _contentStatus = 2;
            _contentSourceSize.width = _contentItem->width;
            _contentSourceSize.height = _contentItem->height;

            _interval = _contentItem->animation->_interval;
            _frameCount = _contentItem->animation->_frameCount;

            CALL_PER_FRAME(GLoader, update);
//            if (_playAction == nullptr)
//            {
//                _playAction = ActionMovieClip::create(_contentItem->animation, _contentItem->repeatDelay);
//                _playAction->retain();
//            }
//            else
//                _playAction->setAnimation(_contentItem->animation, _contentItem->repeatDelay);
//            if (_playing)
//                _content->runAction(_playAction);
//            else
//                _playAction->setCurrentFrame(_frame);

            updateLayout();
        }
        else
        {
            if (_autoSize)
                setSize(_contentItem->width, _contentItem->height);

            setErrorState();
        }
    }
    else
        setErrorState();
}

void GLoader::loadExternal()
{
    SpriteFrame* sf = nullptr;
    //auto tex = Director::getInstance()->getTextureCache()->addImage(_url);
    if (sf)
    {
//        auto sf = SpriteFrame::createWithTexture(tex, Rect(Vec2::ZERO, tex->getContentSize()));
        onExternalLoadSuccess(sf);
    }
    else
        onExternalLoadFailed();
}

void GLoader::freeExternal(SpriteFrame* spriteFrame)
{
}

void GLoader::onExternalLoadSuccess(SpriteFrame* spriteFrame)
{
    _contentStatus = 4;
    this->setSpriteFrame(spriteFrame);
    Size sz = spriteFrame->getRectInPixels().size;
    _contentSourceSize.width = sz.width;
    _contentSourceSize.height = sz.height;
    updateLayout();
}

void GLoader::onExternalLoadFailed()
{
    setErrorState();
}

void GLoader::clearContent()
{
    clearErrorState();

    if (_contentStatus == 4)
        freeExternal(_spriteFrame);

    if (_contentStatus == 2)
    {
//        _playAction->setAnimation(nullptr);
//        _content->stopAction(_playAction);
        CALL_PER_FRAME_CANCEL(GLoader, update);
    }

//    ((FUISprite*)_content)->clearContent();
    _contentItem = nullptr;

    _contentStatus = 0;
}

void GLoader::updateLayout()
{
    if (_contentStatus == 0)
    {
        if (_autoSize)
        {
            _updatingLayout = true;
            setSize(50, 30);
            _updatingLayout = false;
        }
        return;
    }

    _contentSize = _contentSourceSize;

    if (_autoSize)
    {
        _updatingLayout = true;
        if (_contentSize.width == 0)
            _contentSize.width = 50;
        if (_contentSize.height == 0)
            _contentSize.height = 30;
        setSize(_contentSize.width, _contentSize.height);
        _updatingLayout = false;

        if (_size.equals(_contentSize))
        {
            this->setScale(1, 1);
            this->setAnchorPoint(Vec2::ZERO);
            this->setPosition(0, 0);
            return;
        }
    }

    float sx = 1, sy = 1;
    if (_fill != LoaderFillType::NONE)
    {
        sx = _size.width / _contentSourceSize.width;
        sy = _size.height / _contentSourceSize.height;

        if (sx != 1 || sy != 1)
        {
            if (_fill == LoaderFillType::SCALE_MATCH_HEIGHT)
                sx = sy;
            else if (_fill == LoaderFillType::SCALE_MATCH_WIDTH)
                sy = sx;
            else if (_fill == LoaderFillType::SCALE)
            {
                if (sx > sy)
                    sx = sy;
                else
                    sy = sx;
            }
            _contentSize.width = floor(_contentSourceSize.width * sx);
            _contentSize.height = floor(_contentSourceSize.height * sy);
        }
    }

    this->setAnchorPoint(Vec2::ZERO);

    if (_align == TextHAlignment::CENTER)
        _nx = floor((_size.width - _contentSize.width) / 2 * this->getScale().x);
    else if (_align == TextHAlignment::RIGHT)
        _nx = floor((_size.width - _contentSize.width)* this->getScale().x);
    else
        _nx = 0;
    if (_verticalAlign == TextVAlignment::CENTER)
        _ny = floor((_size.height - _contentSize.height) / 2 * this->getScale().y);
    else if (_verticalAlign == TextVAlignment::BOTTOM)
        _ny = (_size.height - _contentSize.height) * this->getScale().y;
    else
        _ny = 0;

     _position = _initPosition + Vec2(_nx, _ny);
}

Rect GLoader::getRenderSize()
{
    if (_contentItem && _contentItem->type == PackageItemType::MOVIECLIP)
    {
        auto pSpriteFrame = this->getSpriteFrame();
        if (this->getFill() == LoaderFillType::NONE)
            return pSpriteFrame->getRectInPixels();
        else if (this->getFill() == LoaderFillType::SCALE)
        {
            Size curSize = this->getSize();
            Rect renderRect = pSpriteFrame->getRectInPixels();
            Size sourceSize = pSpriteFrame->_size;
            float wValue = curSize.width / sourceSize.width;
            float hValue = curSize.height / sourceSize.height;
            float value = MIN(wValue, hValue);
            renderRect.origin.x *= value;
            renderRect.origin.y *= value;
            renderRect.size.width *= value;
            renderRect.size.height *= value;
            return renderRect;
        }
        else if (this->getFill() == LoaderFillType::SCALE_MATCH_HEIGHT)
        {
            Size curSize = this->getSize();
            Rect renderRect = pSpriteFrame->getRectInPixels();
            Size sourceSize = pSpriteFrame->_size;
            float wValue = curSize.width / sourceSize.width;
            float hValue = curSize.height / sourceSize.height;

            renderRect.origin.x *= hValue;
            renderRect.origin.y *= hValue;
            renderRect.size.width *= hValue;
            renderRect.size.height *= hValue;
            return renderRect;
        }
        else if (this->getFill() == LoaderFillType::SCALE_MATCH_WIDTH)
        {
            Size curSize = this->getSize();
            Rect renderRect = pSpriteFrame->getRectInPixels();
            Size sourceSize = pSpriteFrame->_size;
            float wValue = curSize.width / sourceSize.width;
            float hValue = curSize.height / sourceSize.height;

            renderRect.origin.x *= wValue;
            renderRect.origin.y *= wValue;
            renderRect.size.width *= wValue;
            renderRect.size.height *= wValue;
            return renderRect;
        }
        else if (this->getFill() == LoaderFillType::SCALE_FREE)
        {
            Size curSize = this->getSize();
            Rect renderRect = pSpriteFrame->getRectInPixels();
            Size sourceSize = pSpriteFrame->_size;
            float wValue = curSize.width / sourceSize.width;
            float hValue = curSize.height / sourceSize.height;

            renderRect.origin.x *= wValue;
            renderRect.origin.y *= hValue;
            renderRect.size.width *= wValue;
            renderRect.size.height *= hValue;
            return renderRect;
        }
    }

    return Rect(Vec2::ZERO, _contentSize);
}

void GLoader::handleScaleChanged()
{
    GObject::handleScaleChanged();

    _initScale = _scale;
}


void GLoader::handlePositionChanged()
{
    GObject::handlePositionChanged();

    _initPosition = _position - Vec2(_nx, _ny);
}

void GLoader::setErrorState()
{
}

void GLoader::clearErrorState()
{
}

void GLoader::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (!_updatingLayout)
        updateLayout();
}

void GLoader::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

//    this->setGrayed(_finalGrayed);
}

void GLoader::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    _initPosition = _position;
    _initScale = _scale;

    const char*p;

    p = xml->Attribute("url");
    if (p)
        _url = p;

    p = xml->Attribute("align");
    if (p)
        _align = ToolSet::parseAlign(p);

    p = xml->Attribute("vAlign");
    if (p)
        _verticalAlign = ToolSet::parseVerticalAlign(p);

    p = xml->Attribute("fill");
    if (p)
        _fill = ToolSet::parseFillType(p);

    p = xml->Attribute("color");
    if (p)
        setColor(ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("frame");
    if (p)
        _curFrame = atoi(p);

    p = xml->Attribute("playing");
    if (p)
        _playing = strcmp(p, "false") != 0;

    if (_url.length() > 0)
        loadContent();
    p = xml->Attribute("fillMethod");
    if (p)
    {
        _fillMethod = ToolSet::parseFillMethod(p);
        if (_fillMethod != FillMethod::NONE)
        {
            p = xml->Attribute("fillOrigin");
            if (p)
                _fillOrigin = atof(p);

            p = xml->Attribute("fillClockwise");
            if (p)
                _fillClockwise = strcmp(p, "true") == 0;

            p = xml->Attribute("fillAmount");
            if (p)
                _fillAmount = atof(p) / 100.0f;
        }
    }
}

void GLoader::setScale9Grid(Rect* value)
{
    if (value == nullptr)
    {
        //        setCenterRectNormalized(Rect(0, 0, 1, 1));
        return;
    }

    Rect insets = *value;

    // When Insets == Zero --> we should use a 1/3 of its untrimmed size
    if (insets.equals(Rect::ZERO)) {
        insets = Rect(_size.width / 3.0f,
            _size.height / 3.0f,
            _size.width / 3.0f,
            _size.height / 3.0f);
    }

    // emulate invalid insets. shouldn't be supported, but the original code supported it.
    if (insets.origin.x > _size.width)
        insets.origin.x = 0;
    if (insets.origin.y > _size.height)
        insets.origin.y = 0;
    if (insets.size.width > _size.width)
        insets.size.width = 1;
    if (insets.size.height > _size.height)
        insets.size.height = 1;

    // we have to convert from untrimmed to trimmed
    // Sprite::setCenterRect is using trimmed values (to be compatible with Cocos Creator)
    // Scale9Sprite::setCapInsects uses untrimmed values (which makes more sense)

    // use _rect coordinates. recenter origin to calculate the
    // intersecting rectangle
    // can't use _offsetPosition since it is calculated using bottom-left as origin,
    // and the center rect is calculated using top-left
//    insets.origin.x -= (_size.width - _contentSize.width) / 2;// +_unflippedOffsetPositionFromCenter.x;
//    insets.origin.y -= (_size.height - _contentSize.height) / 2;// -_unflippedOffsetPositionFromCenter.y;

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

void GLoader::update(float delta)
{
    if (!_playing) return;

    if (_frameCount > 0)
    {
        _updatetime += delta;
        if (_updatetime > _interval)
        {
            _updatetime = _updatetime - _interval;
            _curFrame = (++_curFrame) % _frameCount;
            auto anim = _contentItem->animation->_frames[_curFrame];
            if(anim != nullptr)
            {
                this->setSpriteFrame(anim->_spriteframe);
            }
        }
    }
}
NS_FGUI_END