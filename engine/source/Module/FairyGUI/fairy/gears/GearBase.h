#ifndef __GEARBASE_H__
#define __GEARBASE_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "base/TweenFunction.h"

NS_FGUI_BEGIN

class GObject;
class GController;

class GearBase
{
    FUI_RTTI_BASE(GearBase)
public:
    GearBase(GObject* owner);
    virtual ~GearBase();

    GController* getController() const { return _controller; }
    void setController(GController* value);

    virtual void updateFromRelations(float dx, float dy);
    virtual void apply();
    virtual void updateState();

    void setup(TXMLElement * xml);

    static bool disableAllTweenEffect;
    bool tween;
    tweenfunc::TweenType easeType;
    float tweenTime;
    float delay;

protected:
    virtual void addStatus(const AZStd::string&  pageId, const AZStd::string& value);
    virtual void init();

    GObject* _owner;
    GController* _controller;
    uint32_t _displayLockToken;
};

NS_FGUI_END

#endif
