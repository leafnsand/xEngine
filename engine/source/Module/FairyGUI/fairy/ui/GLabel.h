#ifndef __GLABEL_H__
#define __GLABEL_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GLabel : public GComponent
{
    FUI_RTTI_DERIVED(GLabel, GComponent)
public:
    GLabel();
    virtual ~GLabel();

    CREATE_FUNC(GLabel);

    const AZStd::string& getTitle() { return _title; }
    void setTitle(const AZStd::string& value);

    virtual const AZStd::string& getText() const override { return _title; }
    virtual void setText(const AZStd::string& value) override { setTitle(value); }

    virtual const AZStd::string& getIcon() const override { return _icon; }
    virtual void setIcon(const AZStd::string& value) override;

    const Color4B& getTitleColor() const;
    void setTitleColor(const Color4B& value);

    int getTitleFontSize() const;
    void setTitleFontSize(int value);

protected:
    virtual void constructFromXML(TXMLElement* xml) override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;

private:
    GObject* _titleObject;
    GObject* _iconObject;
    AZStd::string _title;
    AZStd::string _icon;
};

NS_FGUI_END

#endif
