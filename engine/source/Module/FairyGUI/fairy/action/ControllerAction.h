#ifndef __CONTROLLERACTION_H__
#define __CONTROLLERACTION_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"

NS_FGUI_BEGIN

class GController;

class ControllerAction
{
public:
    static ControllerAction* createAction(const char* types);

    ControllerAction();
    virtual ~ControllerAction();

    void run(GController* controller, const AZStd::string& prevPage, const AZStd::string& curPage);
    virtual void setup(TXMLElement * xml);

    vector<AZStd::string> fromPage;
    vector<AZStd::string> toPage;

protected:
    virtual void enter(GController* controller) = 0;
    virtual void leave(GController* controller) = 0;
};

NS_FGUI_END

#endif
