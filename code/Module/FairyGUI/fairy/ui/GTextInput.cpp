#include "GTextInput.h"
#include "UIPackage.h"
#include "utils/UBBParser.h"
//#include "input/key.h"
#include "fairy/input/KeyBoard.h"
#include "display/FonsFont.h"

//using namespace Input;

NS_FGUI_BEGIN


GTextInput::GTextInput():
    _isInputing(false),
    _cursor(nullptr)
{
}

GTextInput::~GTextInput()
{
    if(_cursor)
        _cursor->release();
}

void GTextInput::handleInit()
{
//    _input = FUIInput::create(this);
//    _input->retain();
//
//    _displayObject = _input;
}

void GTextInput::applyTextFormat()
{
//    _input->applyTextFormat();
}

void GTextInput::setRestrict(const AZStd::string & value)
{
}

void GTextInput::handleSizeChanged()
{
    _isDirty = true;
//    _input->setContentSize(_size);
    this->setSize(_size.width, _size.height);
}

void GTextInput::setup_BeforeAdd(TXMLElement * xml)
{
    GTextField::setup_BeforeAdd(xml);

    const char *p;

    p = xml->Attribute("prompt");
    if (p)
        setPrompt(p);

    if (xml->BoolAttribute("password"))
        setPassword(true);

    p = xml->Attribute("restrict");
    if (p)
        setRestrict(p);

    p = xml->Attribute("maxLength");
    if (p)
        setMaxLength(atoi(p));

    p = xml->Attribute("keyboardType");
    if (p)
        setKeyboardType(atoi(p));


    addEventListener(UIEventType::KeyDown, FUI_CALLBACK_1(GTextInput::onKeyDown, this));
    addEventListener(UIEventType::KeyUp, FUI_CALLBACK_1(GTextInput::onKeyUp, this));
}

void GTextInput::onKeyDown(EventContext * context)
{
    if(context && context->getInput())
    {
//        Key::Code kc = (Key::Code)(context->getInput()->getKeyCode());
//        KeyBoard::OperateStringKeyDown(_string, kc, _cursorPos);

        //Todo 
        if (_cursor)
            _cursor->SetOffset(Vec2(_cursorPos * 10.0f, 0));

        _isDirty = true;
    }
}

void GTextInput::onKeyUp(EventContext * context)
{
    if (context && context->getInput())
    {
//        Key::Code kc = (Key::Code)(context->getInput()->getKeyCode());
//        KeyBoard::OperateKeyUp(kc);
    }
}

void GTextInput::OnFocusIn()
{
    _isDirty = true;
//    n_printf("GTextInput::OnFocusIn\n");
    _isInputing = true;

    _cursorPos = _string.size();

    if (_cursor == nullptr)
    {
//        _cursor = Cursor::create();
//        _cursor->retain();
//        _cursor->DrawCursor();
//        _cursor->SetParentNode(this);
//        _cursor->SetOffset(Vec2(_cursorPos * 10.0f, 0));
    }

}
void GTextInput::OnFocusOut()
{
    _isDirty = true;
    // n_printf("GTextInput::OnFocusOut\n");
    _isInputing = false;
    dispatchEvent(UIEventType::Submit, this);
}

const AZStd::string& GTextInput::getText() const
{
    return _string;
}

void GTextInput::setText(const AZStd::string& value)
{
    _string = value;
    _isDirty = true;
}

const AZStd::string& GTextInput::getRenderText() const
{
    if (_string.size() == 0 && _isInputing == false)
        return _prompt;

    return _string;
}

void GTextInput::setPrompt(const AZStd::string& value)
{
    _isDirty = true;
    _prompt = value;

//    if (_ubbEnabled)
//        parsedText = UBBParser::getInstance()->parse(_prompt.c_str());
//    else
//        parsedText = _prompt;
}

const TextParser& GTextInput::GetTextParser()
{
    if (!_isDirty) return _textParser;

    if (-1 == FonsFont::GetFonsFontIndex(getTextFormat()->face.c_str())) return _textParser;

    _isDirty = false;

    float limit = this->getSize().width;
//    if (_autoSize != TextAutoSize::NONE && _autoSize != TextAutoSize::HEIGHT)
//        limit = 0;

    AZStd::string text = this->getRenderText();

    _textParser.Begin(limit);
    if (this->isUBBEnabled())
    {
        TagData td;
        td.setDefault([this](TagData& tagData)
        {
            tagData.isBold = getTextFormat()->bold;
            tagData.fontSize = getTextFormat()->fontSize;
            tagData.col = getTextFormat()->color;
            tagData.font = getTextFormat()->face;
        });
        vector<TagData> vecData;
        text = UBBParser::getInstance()->parse(text.c_str(), vecData, td);
        for(auto& it : vecData)
        {
            _textParser.Parser(it, getTextFormat()->letterSpacing);
        }
    }
    else
    {
        _textParser.Parser(text.c_str(), getTextFormat()->face, getTextFormat()->fontSize, getTextFormat()->color, getTextFormat()->bold, getTextFormat()->letterSpacing);
    }
    _textParser.End();

    return _textParser;
}


void GTextInput::RefreshTransform()
{
    GObject::RefreshTransform();

    if(_cursor)
    {
        _cursor->RefreshTransform();
    }
}

NS_FGUI_END