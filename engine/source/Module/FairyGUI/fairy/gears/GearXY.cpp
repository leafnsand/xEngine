#include "GearXY.h"
#include "ui/GObject.h"
#include "utils/ToolSet.h"
#include "ui/UIPackage.h"

NS_FGUI_BEGIN


GearXY::GearXY(GObject * owner) : GearBase(owner)
{
    action = Action<Vec2>::create();
    action->retain();
}

GearXY::~GearXY()
{
    action->Stop();
    action->release();
}

void GearXY::init()
{
    _default = Vec2(_owner->getX(), _owner->getY());
    _storage.clear();
}

void GearXY::addStatus(const AZStd::string&  pageId, const AZStd::string& value)
{
    if (value == "-" || value.length() == 0)
        return;

    Vec2 v2;
    ToolSet::splitString(value, ',', v2);

    if (pageId.size() == 0)
        _default = v2;
    else
        _storage[pageId] = v2;
}

void GearXY::apply()
{
    Vec2 gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if(action->IsRunning())
        {
            if (_tweenTarget.x != gv.x || _tweenTarget.y != gv.y)
            {
                onTweenComplete();
            }
            else
                return;
        }

        if (_owner->getX() != gv.x || _owner->getY() != gv.y)
        {
            if (_owner->checkGearController(0, _controller))
                _displayLockToken = _owner->addDisplayLock();
            _tweenTarget = gv;
            
            action->SetTweenData(_owner->getPosition(), gv, delay, tweenTime, easeType, FUI_CALLBACK_1(GearXY::onTweenUpdate, this), FUI_CALLBACK_0(GearXY::onTweenComplete, this));
            action->Run();
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setPosition(gv.x, gv.y);
        _owner->_gearLocked = false;
    }
}

void GearXY::onTweenUpdate(const Vec2& v)
{
    _owner->_gearLocked = true;
    _owner->setPosition(v.x, v.y);
    _owner->_gearLocked = false;
}

void GearXY::onTweenComplete()
{
    if (_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_displayLockToken);
        _displayLockToken = 0;
    }
    action->Stop();
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearXY::updateState()
{
    _storage[_controller->getSelectedPageId()] = Vec2(_owner->getX(), _owner->getY());
}

void GearXY::updateFromRelations(float dx, float dy)
{
    if (_controller != nullptr && !_storage.empty())
    {
        for (auto it = _storage.begin(); it != _storage.end(); ++it)
        {
            it->second = Vec2(it->second.x + dx, it->second.y + dy);
        }
        _default.x += dx;
        _default.y += dy;

        updateState();
    }
}

NS_FGUI_END