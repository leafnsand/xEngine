#include "ControllerAction.h"

#include "ChangePageAction.h"
#include "PlayTransitionAction.h"

#include "ui/Controller.h"

#include "utils/SwitchHelper.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN

ControllerAction * ControllerAction::createAction(const char* type)
{
    switch (hash_(type))
    {
    case "play_transition"_hash:
        return new PlayTransitionAction();

    case "change_page"_hash:
        return new ChangePageAction();
    }
    return nullptr;
}

ControllerAction::ControllerAction()
{
}

ControllerAction::~ControllerAction()
{
}

void ControllerAction::run(GController * controller, const AZStd::string & prevPage, const AZStd::string & curPage)
{
    if ((fromPage.empty() || find(fromPage.cbegin(), fromPage.cend(), prevPage) != fromPage.cend())
        && (toPage.empty() || find(toPage.cbegin(), toPage.cend(), curPage) != toPage.cend()))
        enter(controller);
    else
        leave(controller);
}

void ControllerAction::setup(TXMLElement * xml)
{
    const char* p;
    p = xml->Attribute("fromPage");
    if (p)
        ToolSet::splitString(p, ',', fromPage);
    p = xml->Attribute("toPage");
    if (p)
        ToolSet::splitString(p, ',', toPage);
}

NS_FGUI_END
