#ifndef __GLOADER_H__
#define __GLOADER_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
//#include "display/Actions.h"

NS_FGUI_BEGIN

class GLoader : public GObject, public IColorGear, public IAnimationGear
{
    FUI_RTTI_DERIVED(GLoader, GObject)
public:
    GLoader();
    virtual ~GLoader();

    CREATE_FUNC(GLoader);

    IColorGear* GetIColorGear() override { return this; }
    IAnimationGear* GetIAnimationGear() override { return this; }

    const AZStd::string& getURL() const { return _url; }
    void setURL(const AZStd::string& value);

    virtual const AZStd::string& getIcon() const override { return _url; }
    virtual void setIcon(const AZStd::string& value) override { setURL(value); }

    TextHAlignment getAlign() const { return _align; }
    void setAlign(TextHAlignment value);

    TextVAlignment getVerticalAlign() const { return _verticalAlign; }
    void setVerticalAlign(TextVAlignment value);

    bool getAutoSize() const { return _autoSize; }
    void setAutoSize(bool value);

    LoaderFillType getFill() const { return _fill; }
    void setFill(LoaderFillType value);

    const Size& getContentSize() { return _contentSize; }

    const Color4B& getColor() const { return _color4b; }
    void setColor(const Color4B& value) { _color4b = value; this->setAlpha(value.a / 255.0f); }

    bool isPlaying() const override { return _playing; }
    void setPlaying(bool value) override;

    int getCurrentFrame() const override { return _curFrame; }//_playAction->getCurrentFrame(); }
    void setCurrentFrame(int value) override;

    Color4B cg_getColor() const override;
    void cg_setColor(const Color4B& value) override;

    void setAnchorPoint(const Vec2& value) { _anchorPoint = value; }
    void setCascadeOpacityEnabled(bool value) { _cascadeOpcity = value; }

    void setSpriteFrame(SpriteFrame* pFrame) { _spriteFrame = pFrame; }
    SpriteFrame* getSpriteFrame() { return _spriteFrame; }

    Rect getScale9GridRect() const { return _scale9grid; }
    PackageItem* getPackageItem() const { return _contentItem; }

    FillMethod getFillMethod() const { return _fillMethod; }
    void setFillMethod(FillMethod value) { _fillMethod = value; }

    int getfillOrigin() const { return _fillOrigin; }
    void setfillOrigin(int value) { _fillOrigin = value; }

    bool getfillClockwise() const { return _fillClockwise; }
    void setfillClockwise(bool value) { _fillClockwise = value; }

    float getfillAmount() const { return _fillAmount; }
    void setfillAmount(float value) { _fillAmount = value; }

    Rect getRenderSize();
protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void handlePositionChanged() override;
    virtual void handleScaleChanged() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

    virtual void loadExternal();
    virtual void freeExternal(SpriteFrame* spriteFrame);
    void onExternalLoadSuccess(SpriteFrame* spriteFrame);
    void onExternalLoadFailed();

private:
    void loadContent();
    void loadFromPackage();
    void clearContent();
    void updateLayout();
    void setErrorState();
    void clearErrorState();
    void setScale9Grid(Rect* value);

    void update(float delta);
    float _interval;
    int _frameCount;
    float _updatetime;
    int _curFrame;

    Rect _scale9grid;
    AZStd::string _url;
    TextHAlignment _align;
    TextVAlignment _verticalAlign;
    bool _autoSize;
    LoaderFillType _fill;
    bool _updatingLayout;
    PackageItem* _contentItem;
    Size _contentSize;
    Size _contentSourceSize;
    int _contentStatus;
    bool _playing;
    Vec2 _anchorPoint;
    bool _cascadeOpcity;
    SpriteFrame* _spriteFrame;

    float _ny = 0;
    float _nx = 0;

    Vec2 _initPosition;
    Vec2 _initScale;

    FillMethod _fillMethod = FillMethod::NONE;
    int _fillOrigin = 0;
    bool _fillClockwise = true;
    float _fillAmount = 1.0f;
};

NS_FGUI_END

#endif
