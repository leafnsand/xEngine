#ifndef __GEARCOLOR_H__
#define __GEARCOLOR_H__

#include "FairyGUIMacros.h"
#include "base/Base.h"
#include "GearBase.h"
#include "display/Action.h"

NS_FGUI_BEGIN

class GObject;

class IColorGear
{
public:
    virtual Color4B cg_getColor() const = 0;
    virtual void cg_setColor(const Color4B& value) = 0;

    virtual Color4B cg_getOutlineColor() const { return Color4B(0, 0, 0, 0); };
    virtual void cg_setOutlineColor(const Color4B& value) {};
};

class GearColor : public GearBase
{
    FUI_RTTI_DERIVED(GearColor, GearBase)
public:
    GearColor(GObject* owner);
    virtual ~GearColor();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const AZStd::string&  pageId, const AZStd::string& value) override;
    void init() override;

private:
    void onTweenUpdate(const Vec4& v);
    void onTweenComplete();

    class GearColorValue
    {
    public:
        Color4B color;
        Color4B outlineColor;

        GearColorValue();
        GearColorValue(const Color4B& color, const Color4B& outlineColor);
    };

    unordered_map<AZStd::string, GearColorValue> _storage;
    GearColorValue _default;
    Vec4 _tweenTarget;
    Action<Vec4>* action;
};

NS_FGUI_END

#endif
