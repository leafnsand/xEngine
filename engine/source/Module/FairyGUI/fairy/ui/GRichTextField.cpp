#include "FairyGUI/fairy/ui/GRichTextField.h"
#include "utils/UBBParser.h"
#include "display/FonsFont.h"

NS_FGUI_BEGIN


GRichTextField::GRichTextField() :
//    _richText(nullptr),
    _updatingSize(false),
    _autoSize(TextAutoSize::BOTH)
{
}

GRichTextField::~GRichTextField()
{
}

void GRichTextField::handleInit()
{
//    _richText = FUIRichText::create();
//    _richText->retain();
//    _richText->setCascadeOpacityEnabled(true);
//
//    _displayObject = _richText;
}

void GRichTextField::setText(const AZStd::string & value)
{
//    _richText->setText(value);
//    if (_ubbEnabled)
//        parsedText = UBBParser::getInstance()->parse(_string.c_str());
//    else
//        parsedText = _string;
    _isDirty = true;
    _string = value;
    if (!_underConstruct)
        updateSize();
}

void GRichTextField::applyTextFormat()
{
//    _richText->applyTextFormat();
    updateGear(4);
    if (!_underConstruct)
        updateSize();
}

void GRichTextField::setAutoSize(TextAutoSize value)
{
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

    this->setDimensions(_size.width, _size.height);
    if (!_underConstruct)
        updateSize();
}

void GRichTextField::setSingleLine(bool value)
{
}

void GRichTextField::updateSize()
{
    if (_updatingSize)
        return;

    _isDirty = true;
    _updatingSize = true;

    Size sz = this->getSize();
    if (_autoSize == TextAutoSize::BOTH)
        setSize(sz.width, sz.height);
    else if (_autoSize == TextAutoSize::HEIGHT)
        setHeight(sz.height);

    _updatingSize = false;
}

void GRichTextField::handleSizeChanged()
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

void GRichTextField::setup_AfterAdd(TXMLElement* xml)
{
    GTextField::setup_AfterAdd(xml);

    updateSize();
}

const TextParser& GRichTextField::GetTextParser()
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
