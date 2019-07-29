#include "GMovieClip.h"
#include "utils/ToolSet.h"
#include "display/Animation.h"

NS_FGUI_BEGIN


GMovieClip::GMovieClip() :
//    _playAction(nullptr),
//    _content(nullptr),
    _playing(true),
    _updatetime(0),
    _curFrame(0),
    _interval(0),
    _frameCount(0)
{
    _sizeImplType = 1;
    _touchDisabled = true;
    CALL_PER_FRAME(GMovieClip, update);
}

GMovieClip::~GMovieClip()
{
    CALL_PER_FRAME_CANCEL(GMovieClip, update);
//    FUI_SAFE_RELEASE(_playAction);
}

void GMovieClip::handleInit()
{

}

void GMovieClip::setPlaySettings(int start, int end, int times, int endAt, function<void()> completeCallback)
{
    _start = start;
    _end = end;
    _times = times;
    _endAt = endAt;
    _completeCB = completeCallback;
    _curFrame = start;

    if(_frameCount > 0)
    {
        if (_end == -1 || _end > _frameCount - 1)
            _end = _frameCount - 1;

        if (_endAt == -1)
            _endAt = _end;
    }
}

void GMovieClip::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
    }
}

//FlipType GMovieClip::getFlip() const
//{
//    if (_content->isFlippedX() && _content->isFlippedY())
//        return FlipType::BOTH;
//    else if (_content->isFlippedX())
//        return FlipType::HORIZONTAL;
//    else if (_content->isFlippedY())
//        return FlipType::VERTICAL;
//    else
//        return FlipType::NONE;
//}
//
//void GMovieClip::setFlip(FlipType value)
//{
//    _content->setFlippedX(value == FlipType::HORIZONTAL || value == FlipType::BOTH);
//    _content->setFlippedY(value == FlipType::VERTICAL || value == FlipType::BOTH);
//}

//void GMovieClip::setColor(const Color3B & value)
//{
//    _content->setColor(value);
//}
//
//Color4B GMovieClip::cg_getColor() const
//{
//    return (Color4B)_content->getColor();
//}
//
//void GMovieClip::cg_setColor(const Color4B& value)
//{
//    _content->setColor((Color3B)value);
//}

void GMovieClip::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

//    this->setGrayed(_finalGrayed);
}

void GMovieClip::constructFromResource()
{
    sourceSize.width = _packageItem->width;
    sourceSize.height = _packageItem->height;
    initSize = sourceSize;

    setSize(sourceSize.width, sourceSize.height);

    _interval = _packageItem->animation->_interval;
    _frameCount = _packageItem->animation->_frameCount;

    if(_frameCount)
    {
        if (_end == -1 || _end > _frameCount - 1)
            _end = _frameCount - 1;

        if (_endAt == -1)
            _endAt = _end;
    }
}

void GMovieClip::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    const char *p;

    p = xml->Attribute("frame");
    if (p)
        setCurrentFrame(atoi(p));

    p = xml->Attribute("playing");
    if (p)
        setPlaying(strcmp(p, "false") != 0);

    p = xml->Attribute("flip");
    if (p)
        setFlip(ToolSet::parseFlipType(p));

    p = xml->Attribute("color");
    if (p)
        setColor(ToolSet::convertFromHtmlColor(p));
}

void GMovieClip::update(float delta)
{
    if (!_playing) return;

    if(_frameCount > 0)
    {
        _updatetime += delta;
        if(_updatetime > _interval)
        {
            _updatetime = _updatetime - _interval;
            if (_curFrame == _end)
            {
                _curFrame = _start;
                if(_times > 0)
                {
                    _times--;
                    if (_times == 0)
                    {
                        _playing = false;
                        _curFrame = _endAt;
                        if (_completeCB)
                        {
                            _completeCB();
                        }
                    }
                }
            }
            else
            {
                _curFrame = (++_curFrame) % _frameCount;
            }
        }
    }
}

NS_FGUI_END