#include "GearAnimation.h"
#include "ui/GObject.h"
#include "utils/ToolSet.h"
#include "ui/UIPackage.h"

NS_FGUI_BEGIN


GearAnimation::GearAnimationValue::GearAnimationValue() :frame(0), playing(false)
{

}

GearAnimation::GearAnimationValue::GearAnimationValue(bool playing, int frame)
{
    this->playing = playing;
    this->frame = frame;
}

GearAnimation::GearAnimation(GObject * owner) :GearBase(owner)
{

}

GearAnimation::~GearAnimation()
{
}

void GearAnimation::init()
{
    IAnimationGear *ag = _owner->GetIAnimationGear();;

    _default = GearAnimationValue(ag->isPlaying(), ag->getCurrentFrame());
    _storage.clear();
}

void GearAnimation::addStatus(const AZStd::string&  pageId, const AZStd::string& value)
{
    if (value == "-" || value.length() == 0)
        return;

    vector<AZStd::string> arr;
    ToolSet::splitString(value, ',', arr);

    GearAnimationValue gv;
    gv.frame = Value(arr[0]).asInt();
    gv.playing = arr[1] == "p";
    if (pageId.empty())
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearAnimation::apply()
{
    _owner->_gearLocked = true;

    GearAnimationValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    IAnimationGear *ag = _owner->GetIAnimationGear();;

    ag->setPlaying(gv.playing);
    ag->setCurrentFrame(gv.frame);

    _owner->_gearLocked = false;
}

void GearAnimation::updateState()
{
    IAnimationGear *ag = _owner->GetIAnimationGear();;

    _storage[_controller->getSelectedPageId()] = GearAnimationValue(ag->isPlaying(), ag->getCurrentFrame());
}

NS_FGUI_END