#ifndef __GIMAGE_H__
#define __GIMAGE_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class GImage : public GObject, public IColorGear
{
    FUI_RTTI_DERIVED(GImage, GObject)
public:
    GImage();
    virtual ~GImage();

    CREATE_FUNC(GImage);

    IColorGear* GetIColorGear() override { return this; }

    FlipType getFlip() const { return _flipType; }
    void setFlip(FlipType value) { _flipType = value; }

    FillMethod getFillMethod() const { return _fillMethod; }
    void setFillMethod(FillMethod value) { _fillMethod = value; }

    int getfillOrigin() const { return _fillOrigin; }
    void setfillOrigin(int value) { _fillOrigin = value; }

    bool getfillClockwise() const { return _fillClockwise; }
    void setfillClockwise(bool value) { _fillClockwise = value; }

    float getfillAmount() const { return _fillAmount; }
    void setfillAmount(float value) { _fillAmount = value; }

    const Color4B& getColor() const { return _color4b; }
    void setColor(const Color4B& value) { _color4b = value; this->setAlpha(value.a / 255.0f); }

    virtual void constructFromResource() override;

    Color4B cg_getColor() const override;
    void cg_setColor(const Color4B& value) override;

    Rect getScale9GridRect() const { return _scale9grid; }
protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

private:
    void setTextureRect(const Rect& rect) { _texRect = rect; }
    void setContentSize(const Size& size) { _size = size; }
    void setScale9Grid(Rect* value);
    void setSpriteFrame(SpriteFrame* pFrame) { _spriteFrame = pFrame; }

    FlipType _flipType = FlipType::NONE;
    Rect _texRect;
    SpriteFrame* _spriteFrame;
    Rect _scale9grid;
    FillMethod _fillMethod = FillMethod::NONE;
    int _fillOrigin = 0;
    bool _fillClockwise = true;
    float _fillAmount = 1.0f;
};

NS_FGUI_END

#endif
