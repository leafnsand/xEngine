#include "TextParser.h"
#include "display/FonsFont.h"

NS_FGUI_BEGIN
//--------------------------------------------------------------------------------------------------------------------
const int ChineseCharCount = 3;

int TextParser::GetLastUtf8Char(const char* text, int offset)
{
    unsigned int utf8state = 0;
    unsigned int codepoint = 0;

    int lastValue = offset - ChineseCharCount;
    lastValue = MAX(lastValue, 0);
    int value = lastValue;
    int whileCount = 0;
    bool isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + value);
    if (isUtf8)
    {
        while (isUtf8)
        {
            whileCount++;
            value++;
            isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + value);
            if (whileCount > ChineseCharCount)
            {
                whileCount = 0;
                utf8state = 0;
                codepoint = 0;
                lastValue = lastValue - 1;
                value = lastValue;
            }
        }
    }
    else
    {
        return value - 1;
    }

    return lastValue;
}

int TextParser::GetNextUtf8Char(const char* text, int offset)
{
    unsigned int utf8state = 0;
    unsigned int codepoint = 0;

    int lastValue = offset;
    int value = lastValue;
    bool isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + value);
    if(isUtf8)
    {
        int whileCount = 1;
        while (isUtf8)
        {
            value++; 
            isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + value);
            if(isUtf8 == false && whileCount == 0)
            {
                return value;
            }
            whileCount++;
            if(whileCount > ChineseCharCount)
            {
                whileCount = 0;
                utf8state = 0;
                codepoint = 0;
                value = lastValue;
            }
        }
    }
    
    return value + 1;
}

bool TextParser::IsUtf8Char(const char* text, int offset)
{
    unsigned int utf8state = 0;
    unsigned int codepoint = 0;
    
    return FonsFont::Decutf8(utf8state, codepoint, text + offset);
}

int TextParser::GetUtf8Head(const char* text, int offset)
{
    unsigned int utf8state = 0;
    unsigned int codepoint = 0;

    bool isUtf8 = true;
    //offset at first
    for(int i = 0; i <= 2; i++)
    {
        isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + offset + i);
    }
    if (isUtf8 == false) return offset;

    utf8state = 0; codepoint = 0;
    //offset at second
    for (int i = -1; i <= 1; i++)
    {
        isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + offset + i);
    }
    if (isUtf8 == false) return offset - 1;

    utf8state = 0; codepoint = 0;
    //offset at third
    for (int i = -2; i <= 0; i++)
    {
        isUtf8 = FonsFont::Decutf8(utf8state, codepoint, text + offset + i);
    }
    if (isUtf8 == false) return offset - 2;

    return offset;
}

int TextParser::GetLongTextNum(const char* text)
{
    int textNum = 0;

    unsigned int utf8state = 0;
    unsigned int codepoint = 0;

    for (; text != text + strlen(text); ++text)
    {
        if (FonsFont::Decutf8(utf8state, codepoint,text))
        {
            continue;
        }
        textNum += 1;
    }
    return textNum;
}

void TextParser::DealLongText(AZStd::string text, bool isNewLine)
{  
    Size lineSize = Size::ZERO;
    if(text.empty())
    {
        lineSize.height = FonsFont::GetTextBounds(_face, _fontSize, "1").height;

        LineParser lp;
        lp._text = text;
        lp._size = lineSize;
        lp._color = _color;
        lp._newLine = isNewLine;
        lp._fontSize = _fontSize;
        lp._bold = _bold;

        this->RefreshCurrentWidthAndHeight(isNewLine, lp._size.width, lp._size.height);

        _vecLine.push_back(lp);
        return;
    }

    lineSize = FonsFont::GetTextBounds(_face, _fontSize, text.c_str());
    int textNum = GetLongTextNum(text.c_str());
    int totalTextSpacing = (textNum - 1) * _letterSpacing;
    totalTextSpacing = MAX(totalTextSpacing, 0);
    lineSize.width = lineSize.width + totalTextSpacing;
    if ((_limitWidth > 0 && (_currentWidth + lineSize.width) <= _limitWidth) || _limitWidth == 0)
    {
        LineParser lp;
        lp._text = text;
        lp._size = lineSize;
        lp._color = _color;
        lp._newLine = isNewLine;
        lp._fontSize = _fontSize;
        lp._bold = _bold;

        this->RefreshCurrentWidthAndHeight(isNewLine, lp._size.width, lp._size.height);

        _vecLine.push_back(lp);
    }
    else
    {
        if (lineSize.width > 0)
        {
            float extra = (_limitWidth - _currentWidth ) / lineSize.width;
            int totalLen = text.size();
            int len = totalLen * extra;
            bool isMore = false;
            bool isLess = false;

            bool isUtf8 = IsUtf8Char(text.c_str(), len);
            if(isUtf8)
            {
                len = GetUtf8Head(text.c_str(), len);
                len += 3;
            }
            while (len > 0)
            {
                lineSize = FonsFont::GetTextBounds(_face, _fontSize, text.c_str(), text.c_str() + len);
                if (lineSize.width > (_limitWidth - _currentWidth))
                {
                    isMore = true;
                    len = len - 1;
                    isUtf8 = IsUtf8Char(text.c_str(), len);
                    if (isUtf8)
                    {
                        len = GetUtf8Head(text.c_str(), len);
                    }
                    if (isLess) break;
                }
                else if (lineSize.width < (_limitWidth - _currentWidth))
                {
                    if (isMore) break;
                    isLess = true;
                    auto lastLen = len;
                    len = len + 1;
                    isUtf8 = IsUtf8Char(text.c_str(), len);
                    if (isUtf8)
                    {
                        len = GetUtf8Head(text.c_str(), len);
                        len += 3;
                    }
                    len = MIN(len, totalLen);
                    if (len == totalLen)
                    {
                        len = lastLen;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }

            AZStd::string ss = "";
            ss.assign(text.c_str(), len);

            LineParser lp;
            lp._text = ss;
            lp._size = lineSize;
            lp._color = _color;
            lp._newLine = true;
            lp._fontSize = _fontSize;
            lp._bold = _bold;

            this->RefreshCurrentWidthAndHeight(true, lp._size.width, lp._size.height);

            _vecLine.push_back(lp);

            ss = text.c_str() + len;
            this->DealLongText(ss, isNewLine);
        }
    }
}

void TextParser::RefreshCurrentWidthAndHeight(bool isNewLine, float width, float& height)
{
    float value = MAX(height, _currentHeight);
    if(_currentHeight != height)
    {
        auto it = _vecLine.end();
        while (it != _vecLine.begin())
        {
            --it;
            if (it->_newLine)
            {
                break;
            }
            else
            {
                it->_size.height = value;
            }
        }
        if (!isNewLine)
        {
            _allSize.height += (value - _currentHeight);
        }
        _currentHeight = value;
    }

    height = _currentHeight;

    if (isNewLine)
    {
        _allSize.height += value;
        _allSize.width = MAX(_allSize.width, width);

        _currentWidth = 0;
        _currentHeight = 0;
    }
    else
    {
        _currentWidth += width;
    }
    _allSize.width = MAX(_allSize.width, _currentWidth);
}

void TextParser::Begin(float limitWidth)
{
    _beginParser = true;
    _limitWidth = limitWidth;
    _vecLine.clear();
    _allSize = Size::ZERO;
    _currentWidth = 0;
    _currentHeight = 0;
}

void TextParser::End()
{
    _beginParser = false;
}

void TextParser::Parser(TagData& textData,int letterSpacing)
{
    this->Parser(textData.value.c_str(), textData.font, textData.fontSize, textData.col, textData.isBold, letterSpacing);
}

void TextParser::Parser(const char* text,AZStd::string face, float fontSize, Color4B color, bool bold,int letterSpacing)
{
    f_assert(_beginParser);

    _text = text;
    _face = face;
    _fontSize = fontSize;
    _color = color;
    _bold = bold;
    _letterSpacing = letterSpacing;

    auto iter = text;
    AZStd::string lineStr = "";
    int count = 0;

    while(*text != '\0')
    {
        ++count;
        if(*text == '\n')
        {
            lineStr.assign(iter, count - 1);
            this->DealLongText(lineStr, true);

            iter += count;
            count = 0;
        }
        ++text;
    }

    if(count > 0)
    {
        lineStr.assign(iter, count);
        this->DealLongText(lineStr, false);
    }
}

NS_FGUI_END