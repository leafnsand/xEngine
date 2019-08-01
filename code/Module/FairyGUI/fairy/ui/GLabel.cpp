#include "GLabel.h"
#include "GButton.h"
#include "GTextField.h"
#include "GTextInput.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN


GLabel::GLabel() :
    _titleObject(nullptr),
    _iconObject(nullptr)
{
}
GLabel::~GLabel()
{
}

void GLabel::setTitle(const AZStd::string & value)
{
    _title = value;
    if (_titleObject != nullptr)
        _titleObject->setText(_title);
    updateGear(6);
}

void GLabel::setIcon(const AZStd::string & value)
{
    _icon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon(_icon);
    updateGear(7);
}

const Color4B& GLabel::getTitleColor() const
{
    if (_titleObject->SafeCast<GTextField>())
        return ((GTextField*)_titleObject)->getColor();
    else if (_titleObject->SafeCast<GLabel>())
        return ((GLabel*)_titleObject)->getTitleColor();
    else if (_titleObject->SafeCast<GButton>())
        return ((GButton*)_titleObject)->getTitleColor();
    else
        return Color4B::BLACK;
}

void GLabel::setTitleColor(const Color4B & value)
{
    if (_titleObject->SafeCast<GTextField>())
        ((GTextField*)_titleObject)->setColor(value);
    else if (_titleObject->SafeCast<GLabel>())
        ((GLabel*)_titleObject)->setTitleColor(value);
    else if (_titleObject->SafeCast<GButton>())
        ((GButton*)_titleObject)->setTitleColor(value);
}

int GLabel::getTitleFontSize() const
{
    if (_titleObject->SafeCast<GTextField>())
        return ((GTextField*)_titleObject)->getFontSize();
    else if (_titleObject->SafeCast<GLabel>())
        return ((GLabel*)_titleObject)->getTitleFontSize();
    else if (_titleObject->SafeCast<GButton>())
        return ((GButton*)_titleObject)->getTitleFontSize();
    else
        return 0;
}

void GLabel::setTitleFontSize(int value)
{
    if (_titleObject->SafeCast<GTextField>())
        ((GTextField*)_titleObject)->setFontSize(value);
    else if (_titleObject->SafeCast<GLabel>())
        ((GLabel*)_titleObject)->setTitleFontSize(value);
    else if (_titleObject->SafeCast<GButton>())
        ((GButton*)_titleObject)->setTitleFontSize(value);
}

void GLabel::constructFromXML(TXMLElement * xml)
{
    GComponent::constructFromXML(xml);

    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();
}

void GLabel::setup_AfterAdd(TXMLElement * xml)
{
    GComponent::setup_AfterAdd(xml);

    xml = xml->FirstChildElement("Label");
    if (!xml)
        return;

    const char *p;

    p = xml->Attribute("title");
    if (p)
        this->setTitle(p);

    p = xml->Attribute("icon");
    if (p)
        this->setIcon(p);

    p = xml->Attribute("titleColor");
    if (p)
        setTitleColor(ToolSet::convertFromHtmlColor(p));

    p = xml->Attribute("titleFontSize");
    if (p)
        setTitleFontSize(atoi(p));

    if(_titleObject)
    {
        GTextInput* input = _titleObject->SafeCast<GTextInput>();
        if (input)
        {
            p = xml->Attribute("prompt");
            if (p)
                input->setPrompt(p);

            if (xml->BoolAttribute("password"))
                input->setPassword(true);

            p = xml->Attribute("restrict");
            if (p)
                input->setRestrict(p);

            p = xml->Attribute("maxLength");
            if (p)
                input->setMaxLength(atoi(p));

            p = xml->Attribute("keyboardType");
            if (p)
                input->setKeyboardType(atoi(p));
        }
    }
}

NS_FGUI_END