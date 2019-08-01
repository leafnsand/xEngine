#ifndef __GGRAPH_H__
#define __GGRAPH_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GGraph : public GObject, public IColorGear
{
    FUI_RTTI_DERIVED(GGraph, GObject)
public:
    GGraph();
    virtual ~GGraph();

    CREATE_FUNC(GGraph);

    IColorGear* GetIColorGear() override { return this; }

    void drawRect(float aWidth, float aHeight, int lineSize, const Color4F& lineColor, const Color4F& fillColor);
    void drawEllipse(float aWidth, float aHeight, int lineSize, const Color4F& lineColor, const Color4F& fillColor);
    bool isEmpty() const { return _type == 0; }

    Color4B cg_getColor() const override;
    void cg_setColor(const Color4B& value) override;

//    GFX::CanvasId GetCanvasId() { return _canvasId; }

    void RefreshTransform() override;
    AZ::Transform GetLocalMatrix44() override;
    Vec2 localToGlobal(const Vec2& pt) override;

    int GetType() const { return _type; }
    Color4F GetLineColor() const { return _lineColor; }
    Color4F GetFillColor() const { return _fillColor; }
    int GetLineSize() const { return _lineSize; }

protected:
    virtual void handleInit() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;
    virtual void handleSizeChanged() override;
    virtual void handlePositionChanged() override;
    virtual void handleScaleChanged() override;

    
private:
    void updateShape();

    int _type;
    Color4F _lineColor;
    Color4F _fillColor;
    int _lineSize;
//    GFX::CanvasId _canvasId;
    bool _isDirty;
};

NS_FGUI_END

#endif
