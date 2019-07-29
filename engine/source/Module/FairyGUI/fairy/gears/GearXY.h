#ifndef __GEARXY_H__
#define __GEARXY_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GearBase.h"
#include "display/Action.h"

NS_FGUI_BEGIN

class GObject;

class GearXY : public GearBase
{
    FUI_RTTI_DERIVED(GearXY, GearBase)
public:
    GearXY(GObject* owner);
    virtual ~GearXY();

    void apply() override;
    void updateState() override;
    void updateFromRelations(float dx, float dy) override;

protected:
    void addStatus(const AZStd::string&  pageId, const AZStd::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const Vec2& v);
    void onTweenComplete();

    unordered_map<AZStd::string, Vec2> _storage;
    Vec2 _default;
    Vec2 _tweenTarget;
    Action<Vec2>* action;
    
};

NS_FGUI_END

#endif
