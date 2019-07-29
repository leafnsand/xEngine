#ifndef __CHANGEPAGEACTION_H__
#define __CHANGEPAGEACTION_H__

#include "FairyGUIMacros.h"

#include "ControllerAction.h"

NS_FGUI_BEGIN

class ChangePageAction : public ControllerAction
{
public:
    virtual void setup(TXMLElement * xml) override;

    AZStd::string objectId;
    AZStd::string controllerName;
    AZStd::string targetPage;

protected:
    virtual void enter(GController* controller) override;
    virtual void leave(GController* controller) override;
};

NS_FGUI_END

#endif
