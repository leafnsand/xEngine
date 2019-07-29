#pragma once

#include "FairyGUIMacros.h"
#include "display/SpriteFrame.h"

NS_FGUI_BEGIN

class AnimationFrame
{
public:
    static AnimationFrame* create(SpriteFrame* spriteframe, float delay);

    SpriteFrame* _spriteframe;
    float _delay;
};

class Animation
{
public:
    static Animation* create();
    void initWithAnimationFrames(vector<AnimationFrame*> frames, float interval, int i);

    vector<AnimationFrame*> _frames;
    float _interval;
    int _frameCount;
};

NS_FGUI_END
