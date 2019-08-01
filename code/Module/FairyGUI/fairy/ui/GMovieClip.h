#ifndef __GMOVIECLIP_H__
#define __GMOVIECLIP_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GMovieClip : public GObject, public IColorGear, public IAnimationGear
{
    FUI_RTTI_DERIVED(GMovieClip, GObject)
public:
    GMovieClip();
    virtual ~GMovieClip();

    CREATE_FUNC(GMovieClip);

    IColorGear* GetIColorGear() override { return this; }
    IAnimationGear* GetIAnimationGear() override { return this; }

    bool isPlaying() const override { return _playing; }
    void setPlaying(bool value) override;

    int getCurrentFrame() const override { return _curFrame; }
    void setCurrentFrame(int value) override { _curFrame = value; }

    FlipType getFlip() const { return _flipType; }
    void setFlip(FlipType value) { _flipType = value; }

    const Color4B& getColor() const { return _color4b; }
    void setColor(const Color4B& value) { _color4b = value; this->setAlpha(value.a / 255.0f); }

    //from start to end(-1 means ending)��repeat times(0 means infinite loop)��when all is over, stopping at endAt(-1 means same value of end)
    void setPlaySettings(int start = 0, int end = -1, int times = 0, int endAt = -1, function<void()> completeCallback = nullptr);

    virtual void constructFromResource() override;

    Color4B cg_getColor() const override { return _color4b; }
    void cg_setColor(const Color4B& value) override { _color4b = value;}

protected:
    virtual void handleInit() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;
    virtual void handleGrayedChanged() override;

    void update(float delta);

private:
//    Sprite* _content;
//    ActionMovieClip* _playAction;
    bool _playing = true;
    int _curFrame = 0;
    FlipType _flipType = FlipType::NONE;
    float _interval = 0;
    int _frameCount = 0;
    float _updatetime = 0;

    int _start = 0;
    int _end = -1;
    int _times = 0;
    int _endAt = -1;
    function<void()> _completeCB;
};

NS_FGUI_END

#endif
