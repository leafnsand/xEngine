#include "UBBParser.h"

NS_FGUI_BEGIN

using namespace AZStd;

FUI__Implement_Singleton(UBBParser)

UBBParser::UBBParser():
    defaultImgWidth(0), 
    defaultImgHeight(0), 
    _pString(nullptr),
    _readPos(0)
{
    _handlers["url"] = UBB_TAG_HANDLER(UBBParser::onTag_URL, this);
    _handlers["img"] = UBB_TAG_HANDLER(UBBParser::onTag_IMG, this);
    _handlers["b"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["i"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["u"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["sup"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["sub"] = UBB_TAG_HANDLER(UBBParser::onTag_Simple, this);
    _handlers["color"] = UBB_TAG_HANDLER(UBBParser::onTag_COLOR, this);
    _handlers["font"] = UBB_TAG_HANDLER(UBBParser::onTag_FONT, this);
    _handlers["size"] = UBB_TAG_HANDLER(UBBParser::onTag_SIZE, this);
    _handlers["align"] = UBB_TAG_HANDLER(UBBParser::onTag_ALIGN, this);
}

UBBParser::~UBBParser()
{

}

AZStd::string UBBParser::parse(const char * text, vector<TagData>& vecData, TagData& tagData)
{
    _pString = text;
    _readPos = 0;

    ssize_t pos;
    bool end;
    string tag, attr;
    string repl;
    string out = text;
    int refCount = 0;
    bool _inBlock = false;

    tagData.clear();
    while (*_pString != '\0')
    {
        const char* p = strchr(_pString, '[');
        if (!p)
        {
            tagData.value = _pString;
            vecData.push_back(tagData);
            tagData.clear();
            break;
        }
        
        pos = p - _pString;
        if(pos > 0)
        {
            tagData.value.append(_pString, pos);
            if(_inBlock == false)
            {
                vecData.push_back(tagData);
                tagData.clear();
            }
            _pString += pos;
        }
        
        p = strchr(_pString, ']');
        if (!p)
        {
            tagData.value = _pString;
            vecData.push_back(tagData);
            tagData.clear();
            break;
        }

        pos = p - _pString;
        end = _pString[1] == '/';
        if (end)
            tag.assign(_pString + 2, pos - 2);
        else
            tag.assign(_pString + 1, pos - 1);
        _readPos = pos + 1;

        attr.clear();
        repl.clear();
        pos = tag.find('=');
        if (pos != -1)
        {
            attr = tag.substr(pos + 1);
            tag = tag.substr(0, pos);
        }

        if(!end)
        {
            refCount++;
            _inBlock = true;

            transform(tag.begin(), tag.end(), tag.begin(), ::tolower);
            auto it = _handlers.find(tag);
            if (it != _handlers.end())
            {
                it->second(tag, end, attr, repl, tagData);
            }
        }
        else
        {
            refCount--;
            if(refCount == 0)
            {
                _inBlock = false;
                vecData.push_back(tagData);
                tagData.clear();
            }
        }
        _pString += _readPos;
    }
    return out;
}

void UBBParser::getTagText(AZStd::string& out, bool remove)
{
    const char* p = strchr(_pString + _readPos, '[');
    if (!p)
        return;

    ssize_t pos =  p - _pString;
    out.assign(_pString, _readPos, pos - _readPos);
    if (remove)
        _readPos = pos;
}

void UBBParser::onTag_URL(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    data.type |= TagData::TagDataType::Url;
    if (!end)
    {
        if (!attr.empty())
        {
            replacement = "<a href=\"" + attr + "\" target=\"_blank\">";
            data.value = attr;
        }
        else
        {
            string href;
            getTagText(href, false);
            replacement = "<a href=\"" + href + "\" target=\"_blank\">";
            data.path = href;
        }
    }
    else
        replacement = "</a>";
}

void UBBParser::onTag_IMG(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    data.type |= TagData::TagDataType::Img;
    if (!end)
    {
        string src;
        getTagText(src, true);
        if (src.empty())
            return;

        if (defaultImgWidth != 0)
        {
            replacement = "<img src=\"" + src + "\" width=\"" + Value(defaultImgWidth).asString() + "\" height=\"" + Value(defaultImgHeight).asString() + "\"/>";
            data.type |= TagData::TagDataType::Size;
            data.size = Vec2(defaultImgWidth, defaultImgHeight);
        }
        else
            replacement = "<img src=\"" + src + "\"/>";

        data.path = src;
    }
}

void UBBParser::onTag_Simple(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    if(!end && tagName == "b")
    {
        data.isBold = true;
    }
    replacement = end ? ("</" + tagName + ">") : ("<" + tagName + ">");
}

void UBBParser::onTag_COLOR(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    data.type |= TagData::TagDataType::Color;
    if (!end)
    {
        string col = attr;
        replacement = "<font color=\"" + attr + "\">";
        if (attr.at(0) == '#') {
            col = col.erase(0, 1);
        }
        
        int len = col.size();
        if(len >= 2)
        {
            string c = "0x" + col.substr(0, 2);
            data.col.r = strtoul(c.c_str(), 0, 16);
        }
        if (len >= 4)
        {
            string c = "0x" + col.substr(2, 2);
            data.col.g = strtoul(c.c_str(), 0, 16);
        }
        if (len >= 6)
        {
            string c = "0x" + col.substr(4, 2);
            data.col.b = strtoul(c.c_str(), 0, 16);
        }
        if (len >= 8)
        {
            string c = "0x" + col.substr(6, 2);
            data.col.a = strtoul(c.c_str(), 0, 16);
        }
    }
    else
        replacement = "</font>";
}

void UBBParser::onTag_FONT(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    data.type |= TagData::TagDataType::Font;
    if (!end)
    {
        replacement = "<font face=\"" + attr + "\">";
        data.font = attr;
    }
    else
        replacement = "</font>";
}

void UBBParser::onTag_SIZE(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    data.type |= TagData::TagDataType::FontSize;
    if (!end)
    {
        replacement = "<font size=\"" + attr + "\">";
        data.fontSize = atoi(attr.c_str());
    }
    else
        replacement = "</font>";
}

void UBBParser::onTag_ALIGN(const AZStd::string & tagName, bool end, const AZStd::string & attr, AZStd::string& replacement, TagData& data)
{
    data.type |= TagData::TagDataType::Align;
    if (!end)
    {
        replacement = "<p align=\"" + attr + "\">";
        data.align = atoi(attr.c_str());
    }
    else
        replacement = "</p>";
}

NS_FGUI_END
