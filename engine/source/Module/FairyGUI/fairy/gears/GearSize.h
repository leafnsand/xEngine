#ifndef __GEARSIZE_H__
#define __GEARSIZE_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GearBase.h"
#include "display/Action.h"

NS_FGUI_BEGIN

class GObject;

class GearSize : public GearBase
{
    FUI_RTTI_DERIVED(GearSize, GearBase)
public:
    GearSize(GObject* owner);
    virtual ~GearSize();

    void apply() override;
    void updateState() override;
    void updateFromRelations(float dx, float dy) override;

protected:
    void addStatus(const AZStd::string&  pageId, const AZStd::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const Vec4& v);
    void onTweenComplete();

    unordered_map<AZStd::string, Vec4> _storage;
    Vec4 _default;
    Vec4 _tweenTarget;
    Action<Vec4>* action;
};

NS_FGUI_END

#endif
