#include "GTextField.h"
#include "utils/ToolSet.h"
#include "utils/UBBParser.h"
#include "display/FonsFont.h"
#include "display/DrawItem.h"

NS_FGUI_BEGIN

GTextField::GTextField() :
    _isDirty(false)
{
    _textFormat = new TextFormat();
    this->m_displayObject = new DrawItem("GTextField", (GObject*)this);
}

void GTextField::setColor(const Color4B & value)
{
    getTextFormat()->color = value;
    applyTextFormat();
}

void GTextField::setFontSize(float value)
{
    getTextFormat()->fontSize = value;
    applyTextFormat();
}

void GTextField::cg_setColor(const Color4B & value)
{
    getTextFormat()->color = value;
    applyTextFormat();
}

void GTextField::cg_setOutlineColor(const Color4B & value)
{
    getTextFormat()->outlineColor = (Color3B)value;
    applyTextFormat();
}

void GTextField::setup_BeforeAdd(TXMLElement * xml)
{
    GObject::setup_BeforeAdd(xml);

    TextFormat* tf = getTextFormat();
    const char*p;
    p = xml->Attribute("font");
    if (p)
        tf->face = p;

    p = xml->Attribute("fontSize");
    if (p)
        tf->fontSize = atoi(p);

    p = xml->Attribute("color");
    if (p)
        tf->color = ToolSet::convertFromHtmlColor(p);

    p = xml->Attribute("align");
    if (p)
        tf->align = ToolSet::parseAlign(p);

    p = xml->Attribute("vAlign");
    if (p)
        tf->verticalAlign = ToolSet::parseVerticalAlign(p);

    p = xml->Attribute("leading");
    if (p)
        tf->lineSpacing = atoi(p);

    p = xml->Attribute("letterSpacing");
    if (p)
        tf->letterSpacing = atoi(p);

    p = xml->Attribute("ubb");
    if (p)
        setUBBEnabled(strcmp(p, "true") == 0);

    p = xml->Attribute("autoSize");
    if (p)
        setAutoSize(ToolSet::parseTextAutoSize(p));

    p = xml->Attribute("underline");
    if (p)
        tf->underline = strcmp(p, "true") == 0;

    p = xml->Attribute("italic");
    if (p)
        tf->italics = strcmp(p, "true") == 0;

    p = xml->Attribute("bold");
    if (p)
        tf->bold = strcmp(p, "true") == 0;

    p = xml->Attribute("singleLine");
    if (p)
        setSingleLine(strcmp(p, "true") == 0);

    p = xml->Attribute("strokeColor");
    if (p)
    {
        tf->outlineColor = (Color3B)ToolSet::convertFromHtmlColor(p);
        p = xml->Attribute("strokeSize");
        tf->outlineSize = p ? atoi(p) : 1;
        tf->enableEffect(TextFormat::OUTLINE);
    }

    p = xml->Attribute("shadowColor");
    if (p)
    {
        tf->shadowColor = (Color3B)ToolSet::convertFromHtmlColor(p);

        Vec2 offset;
        p = xml->Attribute("shadowOffset");
        if (p)
            ToolSet::splitString(p, ',', offset);
        offset.y = -offset.y;
        tf->shadowOffset = offset;
        tf->enableEffect(TextFormat::SHADOW);
    }
}

void GTextField::setup_AfterAdd(TXMLElement * xml)
{
    GObject::setup_AfterAdd(xml);

    applyTextFormat();

    const char* p;
    p = xml->Attribute("text");
    if (p && strlen(p) > 0)
        setText(p);
}

const TextParser& GTextField::GetTextParser()
{
    return _textParser;
}
//---------------------------

GBasicTextField::GBasicTextField() :
    //    _label(nullptr),
    _autoSize(TextAutoSize::BOTH),
    _updatingSize(false)
{
    _touchDisabled = true;
}

GBasicTextField::~GBasicTextField()
{
}

void GBasicTextField::handleInit()
{
//    _label = FUILabel::create();
//    _label->retain();
//
//    _displayObject = _label;
}

void GBasicTextField::setText(const AZStd::string & text)
{
    _isDirty = true;
//    _label->setText(text);
    _string = text;
//    if (_ubbEnabled)
//        parsedText = UBBParser::getInstance()->parse(_string.c_str());
//    else
//        parsedText = _string;

    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::applyTextFormat()
{
//    this->applyTextFormat();
    updateGear(4);
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setAutoSize(TextAutoSize value)
{
    _isDirty = true;
    _autoSize = value;
    switch (value)
    {
    case TextAutoSize::NONE:
        this->setOverflow(Overflow::CLAMP);
        break;
    case TextAutoSize::BOTH:
        this->setOverflow(Overflow::NONE);
        break;
    case TextAutoSize::HEIGHT:
        this->setOverflow(Overflow::RESIZE_HEIGHT);
        break;
    case TextAutoSize::SHRINK:
        this->setOverflow(Overflow::SHRINK);
        break;
    }

    if (_autoSize == TextAutoSize::BOTH)
        this->setDimensions(0, 0);
    else if (_autoSize == TextAutoSize::HEIGHT)
        this->setDimensions(_size.width, 0);
    else
        this->setDimensions(_size.width, _size.height);

    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::setSingleLine(bool value)
{
    _isDirty = true;
//    _label->enableWrap(!value);
    _isWrap = value;
    if (!_underConstruct)
        updateSize();
}

void GBasicTextField::updateSize()
{
    _isDirty = true;
    if (_updatingSize)
        return;

    _updatingSize = true;

    Size sz = this->getSize();
    if (_autoSize == TextAutoSize::BOTH)
        setSize(sz.width, sz.height);
    else if (_autoSize == TextAutoSize::HEIGHT)
        setHeight(sz.height);

    _updatingSize = false;
}

void GBasicTextField::handleSizeChanged()
{
    if (_updatingSize)
        return;

    if (_autoSize != TextAutoSize::BOTH)
    {
        this->setDimensions(_size.width, _size.height);

        if (_autoSize == TextAutoSize::HEIGHT)
        {
            if (_string.length() > 0)
                setSizeDirectly(_size.width, this->getSize().height);
        }
    }
}

void GBasicTextField::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

//    this->setGrayed(_finalGrayed);
}

void GBasicTextField::setup_AfterAdd(TXMLElement* xml)
{
    GTextField::setup_AfterAdd(xml);

    updateSize();
}

float GBasicTextField::GetShrinkScale()
{
    if (_autoSize == TextAutoSize::SHRINK)
    {
        Size size = _textParser.GetAllSize();
        float wRate = this->_size.width / size.width;
        float hRate = this->_size.height / size.height;
        return MIN(1.0f, MIN(wRate, hRate));
    }
    return 1.0f;
}

const TextParser& GBasicTextField::GetTextParser()
{
    if (!_isDirty) return _textParser;

    if (-1 == FonsFont::GetFonsFontIndex(getTextFormat()->face.c_str())) return _textParser;

    _isDirty = false;

    float limit = this->getSize().width;
    if (_autoSize != TextAutoSize::NONE && _autoSize != TextAutoSize::HEIGHT)
        limit = 0;

    AZStd::string text = _string;

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
        for (auto& it : vecData)
        {
            _textParser.Parser(it, getTextFormat()->letterSpacing);
        }
    }
    else
    {
        _textParser.Parser(text.c_str(), getTextFormat()->face, getTextFormat()->fontSize, getTextFormat()->color, getTextFormat()->bold, getTextFormat()->letterSpacing);
    }
    _textParser.End();

    if (_autoSize == TextAutoSize::HEIGHT)
    {
        this->_size = _textParser.GetAllSize();
        this->_size.width = limit;
    }
    else if (_autoSize == TextAutoSize::BOTH)
    {
        this->_size = _textParser.GetAllSize();
    }

    return _textParser;
}
NS_FGUI_END
