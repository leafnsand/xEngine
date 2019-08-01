#include "Controller.h"
#include "GComponent.h"
#include "utils/ToolSet.h"
#include "action/ControllerAction.h"
//#include "controller_action/ControllerAction.h"

NS_FGUI_BEGIN


GController::GController() :
    changing(false),
    autoRadioGroupDepth(false),
    _parent(nullptr),
    _selectedIndex(-1),
    _previousIndex(-1)
{
}

GController::~GController()
{
    for (auto &it : _actions)
        delete it;
}

void GController::setSelectedIndex(int value)
{
    if (_selectedIndex != value)
    {
        f_assert2(value < (int)_pageIds.size(), "Invalid selected index");

        changing = true;

        _previousIndex = _selectedIndex;
        _selectedIndex = value;
        _parent->applyController(this);

        dispatchEvent(UIEventType::Changed);

        changing = false;
    }
}

const AZStd::string& GController::getSelectedPage() const
{
    if (_selectedIndex == -1)
        return UIConfig::getInstance()->emptyString;
    else
        return _pageNames[_selectedIndex];
}

void GController::setSelectedPage(const AZStd::string & value)
{
    int i = ToolSet::findInStringArray(_pageNames, value);
    if (i == -1)
        i = 0;
    setSelectedIndex(i);
}

const AZStd::string& GController::getSelectedPageId() const
{
    if (_selectedIndex == -1)
        return UIConfig::getInstance()->emptyString;
    else
        return _pageIds[_selectedIndex];
}

void GController::setSelectedPageId(const AZStd::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i != -1)
        setSelectedIndex(i);
}

const AZStd::string& GController::getPreviousPage() const
{
    if (_previousIndex == -1)
        return UIConfig::getInstance()->emptyString;
    else
        return _pageNames[_previousIndex];
}

const AZStd::string& GController::getPreviousPageId() const
{
    if (_previousIndex == -1)
        return UIConfig::getInstance()->emptyString;
    else
        return _pageIds[_previousIndex];
}

int GController::getPageCount() const
{
    return (int)_pageIds.size();
}

bool GController::hasPage(const AZStd::string & aName) const
{
    return ToolSet::findInStringArray(_pageNames, aName) != -1;
}

int GController::getPageIndexById(const AZStd::string & value) const
{
    return ToolSet::findInStringArray(_pageIds, value);
}

const AZStd::string& GController::getPageNameById(const AZStd::string & value) const
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i != -1)
        return _pageNames[i];
    else
        return UIConfig::getInstance()->emptyString;
}

const AZStd::string& GController::getPageId(int index) const
{
    return _pageIds[index];
}

void GController::setOppositePageId(const AZStd::string & value)
{
    int i = ToolSet::findInStringArray(_pageIds, value);
    if (i > 0)
        setSelectedIndex(0);
    else if (_pageIds.size() > 1)
        setSelectedIndex(1);
}

void GController::runActions()
{
    if (_actions.empty())
        return;

    for (auto &it : _actions)
        it->run(this, getPreviousPageId(), getSelectedPageId());
}

void GController::setup(TXMLElement * xml)
{
    const char* p;

    p = xml->Attribute("name");
    if (p)
        name = p;

    autoRadioGroupDepth = xml->BoolAttribute("autoRadioGroupDepth");

    p = xml->Attribute("pages");
    if (p)
    {
        vector<AZStd::string> elems;
        ToolSet::splitString(p, ',', elems);
        int cnt = (int)elems.size();
        for (int i = 0; i < cnt; i += 2)
        {
            _pageIds.push_back(elems[i]);
            _pageNames.push_back(elems[i + 1]);
        }
    }

    TXMLElement* cxml = xml->FirstChildElement("action");
    while (cxml)
    {
        ControllerAction* action = ControllerAction::createAction(cxml->Attribute("type"));
        action->setup(cxml);
        _actions.push_back(action);

        cxml = cxml->NextSiblingElement("action");
    }

    if (_parent != nullptr && _pageIds.size() > 0)
        _selectedIndex = 0;
    else
        _selectedIndex = -1;
}

NS_FGUI_END
