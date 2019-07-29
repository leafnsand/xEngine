#include "Animation.h"


NS_FGUI_BEGIN

AnimationFrame* AnimationFrame::create(SpriteFrame* spriteframe, float delay)
{
    AnimationFrame* anim = new AnimationFrame;
    anim->_spriteframe = spriteframe;
    anim->_delay = delay;
    return anim;
}

//------------------------------------------------------------------------------------------------
Animation* Animation::create()
{
    return new Animation;
}

void Animation::initWithAnimationFrames(vector<AnimationFrame*> frames, float interval, int i)
{
    this->_frames = frames;
    this->_interval = interval;
    this->_frameCount = frames.size();
}


NS_FGUI_END