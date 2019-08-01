#ifndef __GCONTROLLER_H__
#define __GCONTROLLER_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "event/UIEventDispatcher.h"

NS_FGUI_BEGIN

class GComponent;
class ControllerAction;

class GController : public UIEventDispatcher
{
    FUI_RTTI_DERIVED(GController, UIEventDispatcher)
public:
    GController();
    virtual ~GController();

    GComponent* getParent() const { return _parent; }
    void setParent(GComponent* value) { _parent = value; }

    int getSelectedIndex() const { return _selectedIndex; }
    void setSelectedIndex(int value);

    const AZStd::string& getSelectedPage() const;
    void setSelectedPage(const AZStd::string& value);

    const AZStd::string& getSelectedPageId() const;
    void setSelectedPageId(const AZStd::string& value);

    int getPrevisousIndex() const  { return _previousIndex; }
    const AZStd::string& getPreviousPage() const;
    const AZStd::string& getPreviousPageId() const;

    int getPageCount() const;
    bool hasPage(const AZStd::string& aName) const;
    int getPageIndexById(const AZStd::string& value) const;
    const AZStd::string& getPageNameById(const AZStd::string& value) const;
    const AZStd::string& getPageId(int index) const;
    void setOppositePageId(const AZStd::string& value);
    void runActions();

    void setup(TXMLElement* xml);

    AZStd::string name;
    bool changing;
    bool autoRadioGroupDepth;

private:
    GComponent* _parent;
    int _selectedIndex;
    int _previousIndex;
    vector<AZStd::string> _pageIds;
    vector<AZStd::string> _pageNames;
    vector<ControllerAction*> _actions;
};

NS_FGUI_END

#endif
