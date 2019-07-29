#ifndef __GTEXTFIELD_H__
#define __GTEXTFIELD_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "utils/TextParser.h"
// #include "display/DrawItem.h"
//#include "display/FUILabel.h"

NS_FGUI_BEGIN

class GTextField : public GObject, public IColorGear
{
    FUI_RTTI_DERIVED(GTextField, GObject)
public:
    GTextField();

    IColorGear* GetIColorGear() override { return this; }

    virtual bool isUBBEnabled() const { return _ubbEnabled; }
    virtual void setUBBEnabled(bool value) { _ubbEnabled = value; }

    virtual TextAutoSize getAutoSize() const { return TextAutoSize::NONE; }
    virtual void setAutoSize(TextAutoSize value) {};

    virtual bool isSingleLine() const { return false; }
    virtual void setSingleLine(bool value) {};

    virtual TextFormat* getTextFormat() const = 0;
    virtual void applyTextFormat() = 0;

    virtual const Size& getTextSize() { return this->getSize(); }

    const Color4B& getColor() { return getTextFormat()->color; }
    void setColor(const Color4B& value);

    float getFontSize() const { return getTextFormat()->fontSize; }
    void setFontSize(float value);

    Color4B cg_getColor() const override { return _textFormat != nullptr ? (Color4B)getTextFormat()->color : Color4B::WHITE; }
    void cg_setColor(const Color4B& value) override;

    Color4B cg_getOutlineColor() const override { return _textFormat != nullptr ? (Color4B)getTextFormat()->outlineColor : Color4B::WHITE; }
    void cg_setOutlineColor(const Color4B& value) override;

    const Size& getDimensions() const { return _dimensions; }
    void setDimensions(float width, float height){ _dimensions.setSize(width, height); }

    Overflow getOverflow()const { return _overflow; }
    void setOverflow(Overflow overflow) { _overflow = overflow; }
    
    virtual const TextParser& GetTextParser();
protected:
    virtual void setup_BeforeAdd(TXMLElement* xml) override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    TextFormat* _textFormat;
    bool _ubbEnabled;
    AZStd::string parsedText;

    TextParser _textParser;
    bool _isDirty;

private:
    Size _dimensions;
    Overflow _overflow;
};

class GBasicTextField : public GTextField
{
    FUI_RTTI_DERIVED(GBasicTextField, GTextField)
public:
    GBasicTextField();
    virtual ~GBasicTextField();

    CREATE_FUNC(GBasicTextField);

    const AZStd::string& getText() const override { return _string; }
    void setText(const AZStd::string& value) override;

    virtual TextAutoSize getAutoSize() const override { return _autoSize; }
    virtual void setAutoSize(TextAutoSize value) override;

    virtual bool isSingleLine() const override { return _isWrap; }
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _textFormat; }
    virtual void applyTextFormat() override;

    const TextParser& GetTextParser() override;
    float GetShrinkScale();
protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    void updateSize();

private:
//    FUILabel* _label;
    TextAutoSize _autoSize;
    bool _updatingSize;
    AZStd::string _string;
    bool _isWrap;
};

NS_FGUI_END

#endif
