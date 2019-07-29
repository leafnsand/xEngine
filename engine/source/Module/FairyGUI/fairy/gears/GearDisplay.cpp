#include "GearDisplay.h"
#include "ui/Controller.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN


GearDisplay::GearDisplay(GObject * owner) :GearBase(owner)
{
    _displayLockToken = 1;
    _lastValue = false;
    _visible = 0;
}

GearDisplay::~GearDisplay()
{
}

void GearDisplay::apply()
{
    _displayLockToken++;
    if (_displayLockToken == 0)
        _displayLockToken = 1;

    if (pages.size() == 0
        || ToolSet::findInStringArray(pages, _controller->getSelectedPageId()) != -1)
    {
        if (_lastValue == true) return;
        _visible = 1;
        _lastValue = true;
    }
    else
    {
        if (_lastValue == false) return;
        _visible = 0;
        _lastValue = false;
    }
}

void GearDisplay::updateState()
{
}

void GearDisplay::addStatus(const AZStd::string&  pageId, const AZStd::string& value)
{
}

void GearDisplay::init()
{
    pages.clear();
}

uint32_t GearDisplay::addLock()
{
    _visible++;
    return _displayLockToken;
}

void GearDisplay::releaseLock(uint32_t token)
{
    if (token == _displayLockToken)
        _visible--;
}

bool GearDisplay::isConnected()
{
    return _controller == nullptr || _visible > 0;
}

NS_FGUI_END