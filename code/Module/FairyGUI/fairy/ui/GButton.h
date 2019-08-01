#ifndef __GBUTTON_H
#define __GBUTTON_H

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GButton : public GComponent
{
    FUI_RTTI_DERIVED(GButton, GComponent)
public:
    static const char* UP;
    static const char* DOWN;
    static const char* OVER;
    static const char* SELECTED_OVER;
    static const char* DISABLED;
    static const char* SELECTED_DISABLED;

    GButton();
    virtual ~GButton();

    CREATE_FUNC(GButton);

    const AZStd::string& getTitle() { return _title; }
    void setTitle(const AZStd::string& value);

    virtual const AZStd::string& getText() const override { return _title; }
    virtual void setText(const AZStd::string& value) override { setTitle(value); }

    virtual const AZStd::string& getIcon() const override { return _icon; }
    virtual void setIcon(const AZStd::string& value) override;

    const AZStd::string& getSelectedTitle() const { return _selectedTitle; }
    void setSelectedTitle(const AZStd::string& value);

    const AZStd::string& getSelectedIcon() const { return _selectedIcon; }
    void setSelectedIcon(const AZStd::string& value);

    const Color4B& getTitleColor() const;
    void setTitleColor(const Color4B& value);

    int getTitleFontSize() const;
    void setTitleFontSize(int value);

    bool isSelected() const { return _selected; }
    void setSelected(bool value);

    GController* getRelatedController() const { return _relatedController; }
    void setRelatedController(GController* c);

    bool isChangeStateOnClick() { return _changeStateOnClick; }
    void setChangeStateOnClick(bool value) { _changeStateOnClick = value; }

protected:
    virtual void constructFromXML(TXMLElement* xml) override;
    virtual void setup_AfterAdd(TXMLElement* xml) override;
    virtual void handleControllerChanged(GController* c) override;

    void setState(const AZStd::string& value);
    void setCurrentState();

private:
    void onRollOver(EventContext* context);
    void onRollOut(EventContext* context);
    void onTouchBegin(EventContext* context);
    void onTouchEnd(EventContext* context);
    void onClick(EventContext* context);
    void onExit(EventContext* context);

    ButtonMode _mode;
    GObject* _titleObject;
    GObject* _iconObject;
    GController* _buttonController;
    GController* _relatedController;
    AZStd::string _relatedPageId;
    AZStd::string _title;
    AZStd::string _selectedTitle;
    AZStd::string _icon;
    AZStd::string _selectedIcon;
    AZStd::string _sound;
    float _soundVolumeScale;
    bool _selected;
    bool _over;
    bool _down;
    int _downEffect;
    bool _downScaled;
    bool _downGray;
    float _downEffectValue;
    bool _changeStateOnClick;
};

NS_FGUI_END

#endif
