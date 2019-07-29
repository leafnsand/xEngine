#ifndef __GEARLOOK_H__
#define __GEARLOOK_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearLook : public GearBase
{
    FUI_RTTI_DERIVED(GearLook, GearBase)
public:
    GearLook(GObject* owner);
    virtual ~GearLook();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const AZStd::string&  pageId, const AZStd::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const Vec2& v);
    void onTweenComplete();

    class GearLookValue
    {
    public:
        float alpha;
        float rotation;
        bool grayed;
        bool touchable;

        GearLookValue();
        GearLookValue(float alpha, float rotation, bool grayed, bool touchable);
    };

    unordered_map<AZStd::string, GearLookValue> _storage;
    GearLookValue _default;
    Vec2 _tweenTarget;
};

NS_FGUI_END

#endif
