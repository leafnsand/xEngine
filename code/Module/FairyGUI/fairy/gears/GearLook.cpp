#include "GearLook.h"
#include "ui/GObject.h"
#include "utils/ToolSet.h"
#include "ui/UIPackage.h"
#include "display/Action.h"
//#include "display/Actions.h"

NS_FGUI_BEGIN

GearLook::GearLookValue::GearLookValue() :alpha(0), rotation(0), grayed(false), touchable(false)
{
}

GearLook::GearLookValue::GearLookValue(float alpha, float rotation, bool grayed, bool touchable)
{
    this->alpha = alpha;
    this->rotation = rotation;
    this->grayed = grayed;
    this->touchable = touchable;
}

GearLook::GearLook(GObject * owner) :GearBase(owner)
{
}

GearLook::~GearLook()
{
}

void GearLook::init()
{
    _default = GearLookValue(_owner->getAlpha(), _owner->getRotation(),
        _owner->isGrayed(), _owner->isTouchable());
    _storage.clear();
}

void GearLook::addStatus(const AZStd::string&  pageId, const AZStd::string& value)
{
    if (value == "-" || value.length() == 0)
        return;

    vector<AZStd::string> arr;
    ToolSet::splitString(value, ',', arr);

    GearLookValue gv;
    gv.alpha = atof(arr[0].c_str());
    gv.rotation = atof(arr[1].c_str());
    gv.grayed = arr[2] == "1";
    if (arr.size() > 3)
        gv.touchable = arr[3] == "1";

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearLook::apply()
{
    GearLookValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweenTarget.x != gv.alpha || _tweenTarget.y != gv.rotation)
        {
            onTweenComplete();
        }
        else
            return;

        bool a = gv.alpha != _owner->getAlpha();
        bool b = gv.rotation != _owner->getRotation();
        if (a || b)
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();
            _tweenTarget.set(gv.alpha, gv.rotation);

            Action<Vec2>* action = Action<Vec2>::create();
            action->SetTweenData(Vec2(_owner->getAlpha(), _owner->getRotation()), _tweenTarget, delay, tweenTime, easeType, FUI_CALLBACK_1(GearLook::onTweenUpdate, this), FUI_CALLBACK_0(GearLook::onTweenComplete, this));
            action->Run();

            _owner->setGrayed(gv.grayed);
            _owner->setTouchable(gv.touchable);
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setAlpha(gv.alpha);
        _owner->setRotation(gv.rotation);
        _owner->setGrayed(gv.grayed);
        _owner->setTouchable(gv.touchable);
        _owner->_gearLocked = false;
    }
}

void GearLook::onTweenUpdate(const Vec2& v)
{
    _owner->_gearLocked = true;
    _owner->setAlpha(v.x);
    _owner->setRotation(v.y);
    _owner->_gearLocked = false;
}

void GearLook::onTweenComplete()
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearLook::updateState()
{
    _storage[_controller->getSelectedPageId()] = GearLookValue(_owner->getAlpha(), _owner->getRotation(),
        _owner->isGrayed(), _owner->isTouchable());
}


NS_FGUI_END