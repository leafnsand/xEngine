#ifndef __UBBPARSER_H__
#define __UBBPARSER_H__

#include "base/Base.h"
#include "FairyGUIMacros.h"
#include <AzCore/std/functional.h>


NS_FGUI_BEGIN

#define UBB_TAG_HANDLER(__selector__,__target__, ...) bind(&__selector__,__target__, placeholders::_1, placeholders::_2, placeholders::_3, placeholders::_4, placeholders::_5, ##__VA_ARGS__)



struct TagData
{
    typedef AZStd::function<void(TagData& data)> TagDataInitFunc;

    enum TagDataType
    {
        None = 0,
        Url = 1 << 0,
        Img = 1 << 1,
        B = 1 << 2,
        I = 1 << 3,
        U = 1 << 4,
        Sup = 1 << 5,
        Sub = 1 << 6,
        Color = 1 << 7,
        Font = 1 << 8,
        FontSize = 1 << 9,
        Align = 1 << 10,
        Size = 1 << 11,
    };

    int type = TagDataType::None;

    AZStd::string value = "";
    AZStd::string path = ""; //url img 
    Color4B col = Color4B::WHITE;    //color
    float fontSize = 0;     //Font
    Vec2 size = Vec2::ZERO;       //Size
    int align = 0;  //Align
    AZStd::string font = "";  //Font
    bool isBold = false; // B

    TagDataInitFunc _func = nullptr;

    void setDefault(TagDataInitFunc func)
    {
        _func = func;
    }

    void clear()
    {
        this->isBold = false;
        this->value = "";
        this->path = "";
        if(_func)
        {
            _func(*this);
        }
    }
};

class UBBParser
{
    FUI_Declare_Singleton(UBBParser)
public:
    UBBParser();
    virtual ~UBBParser();

    AZStd::string parse(const char *text, vector<TagData>& vecData, TagData& tagData);

    int defaultImgWidth;
    int defaultImgHeight;

protected:
    virtual void onTag_URL(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);
    virtual void onTag_IMG(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);
    virtual void onTag_Simple(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);
    virtual void onTag_COLOR(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);
    virtual void onTag_FONT(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);
    virtual void onTag_SIZE(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);
    virtual void onTag_ALIGN(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data);

    void getTagText(AZStd::string& out, bool remove);

    typedef function<void(const AZStd::string& tagName, bool end, const AZStd::string& attr, AZStd::string& replacement, TagData& data)> TagHandler;
    unordered_map<AZStd::string, TagHandler> _handlers;

    const char* _pString;
    ssize_t _readPos;
    TagData _tagData;

private:
    static UBBParser* _inst;
};

NS_FGUI_END

#endif