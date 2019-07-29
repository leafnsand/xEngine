#ifndef __GRICHTEXTFIELD_H__
#define __GRICHTEXTFIELD_H__

#include "FairyGUI/fairy/base/Base.h"
#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/ui/GTextField.h"
//#include "display/FUIRichText.h"

NS_FGUI_BEGIN

class GRichTextField : public GTextField
{
    FUI_RTTI_DERIVED(GRichTextField, GTextField)
public:
    GRichTextField();
    virtual ~GRichTextField();

    CREATE_FUNC(GRichTextField);

    const AZStd::string& getText() const override { return _string; }
    void setText(const AZStd::string& value) override;

//    virtual bool isUBBEnabled() const override { return _ubbEnabled; }
//    virtual void setUBBEnabled(bool value) override { _ubbEnabled = value; }

    virtual TextAutoSize getAutoSize() const override { return _autoSize; }
    virtual void setAutoSize(TextAutoSize value) override;

    virtual bool isSingleLine() const override { return false; }
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _textFormat; }
    virtual void applyTextFormat() override;
    
    const TextParser& GetTextParser() override;
protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

    void updateSize();

private:
//    FUIRichText* _richText;
    TextAutoSize _autoSize;
    bool _updatingSize;
    AZStd::string _string;
//    bool _ubbEnabled;
};

NS_FGUI_END

#endif
