#ifndef __GEARDISPLAY_H__
#define __GEARDISPLAY_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearDisplay : public GearBase
{
    FUI_RTTI_DERIVED(GearDisplay, GearBase)
public:
    GearDisplay(GObject* owner);
    virtual ~GearDisplay();

    void apply() override;
    void updateState() override;

    uint32_t addLock();
    void releaseLock(uint32_t token);
    bool isConnected();

    vector<AZStd::string> pages;

protected:
    void addStatus(const AZStd::string&  pageId, const AZStd::string& value) override;
    void init() override;

private:
    int _visible;
    bool _lastValue;
};

NS_FGUI_END

#endif
