#ifndef __GPROGRESSBAR_H__
#define __GPROGRESSBAR_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "display/Action.h"

NS_FGUI_BEGIN

class GProgressBar : public GComponent
{
    FUI_RTTI_DERIVED(GProgressBar, GComponent)
public:
    GProgressBar();
    virtual ~GProgressBar();

    CREATE_FUNC(GProgressBar);

    ProgressTitleType getTitleType() const { return _titleType; }
    void setTitleType(ProgressTitleType value);

    double getMax() const { return _max; }
    void setMax(double value);

    double getValue() const { return _value; }
    void setValue(double value);

    void tweenValue(double value, float duration);
    void onTweenUpdate(const float& v);
protected:
    virtual void handleSizeChanged() override;
    virtual void constructFromXML(TXMLElement* xml) override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    void update(double newValue);

private:
    double _max;
    double _value;
    ProgressTitleType _titleType;
    bool _reverse;
    Action<float>* action;

    GObject* _titleObject;
    GObject* _barObjectH;
    GObject* _barObjectV;
    float _barMaxWidth;
    float _barMaxHeight;
    float _barMaxWidthDelta;
    float _barMaxHeightDelta;
    float _barStartX;
    float _barStartY;
};

NS_FGUI_END

#endif
