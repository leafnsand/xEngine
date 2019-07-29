#ifndef __GTEXTINPUT_H__
#define __GTEXTINPUT_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "Cursor.h"
//#include "display/FUIInput.h"

NS_FGUI_BEGIN

class GTextInput : public GTextField
{
    FUI_RTTI_DERIVED(GTextInput, GTextField)
public:
    GTextInput();
    virtual ~GTextInput();

    CREATE_FUNC(GTextInput);

    const AZStd::string& getText() const override;
    void setText(const AZStd::string& value) override;

    virtual bool isSingleLine() const override { return _isSingleLine; }
    virtual void setSingleLine(bool value) override { _isSingleLine = value; }

    virtual TextFormat* getTextFormat() const override { return _textFormat; }
    virtual void applyTextFormat() override;

    void setPrompt(const AZStd::string& value);
    void setPassword(bool value) { _isPassword = value; }
    void setKeyboardType(int value) { _keyboardType = value; }
    void setMaxLength(int value) { _maxLength = value; }
    void setRestrict(const AZStd::string& value);

    void RefreshTransform() override;

    void OnFocusIn();
    void OnFocusOut();
    const TextParser& GetTextParser() override;
    bool IsEditing() { return _isInputing; }
    Cursor* GetCursor() { return _cursor; }
protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_BeforeAdd(TXMLElement* xml) override;

private:
    void onKeyDown(EventContext * context);
    void onKeyUp(EventContext * context);
    const AZStd::string& getRenderText() const;

//    FUIInput* _input;
    bool _isSingleLine;
    AZStd::string _string;
    AZStd::string _prompt;
    bool _isPassword;
    int _keyboardType;
    int _maxLength;

    bool _isInputing;
    Cursor* _cursor;
    int _cursorPos = 0;
};

NS_FGUI_END

#endif
