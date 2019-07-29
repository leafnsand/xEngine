#pragma once

#include "FairyGUIMacros.h"


#include "base/Geometry.h"
#include "UBBParser.h"

NS_FGUI_BEGIN
enum class TextHAlignment;
enum class TextVAlignment;

struct LineParser
{
    AZStd::string _text = "";
    Size _size = Size::ZERO;
    Color4B _color = Color4B::WHITE;
    bool _newLine = false;
    float _fontSize = 0;
    bool _bold = false;
};

class TextParser
{
public:    
    void Begin(float limit = 0);
    void Parser(const char* text, AZStd::string face, float fontSize, Color4B color, bool bold,int letterSpacing);
    void Parser(TagData& textData,int letterSpacing);
    void End();
    Size GetAllSize() {
        return _allSize;
    }

    vector<LineParser> _vecLine;
private:
    void DealLongText(AZStd::string text, bool isNewLine);
    int GetLastUtf8Char(const char* text, int offset);
    int GetNextUtf8Char(const char* text, int offset);
    int GetUtf8Head(const char* text, int offset);
    bool IsUtf8Char(const char* text, int offset);
    void RefreshCurrentWidthAndHeight(bool isNewLine, float width, float& height);
    int GetLongTextNum(const char* text);

    const char* _text = nullptr;
    int _maxLengthLine = 0;
    float _limitWidth = 0;
    float _currentWidth = 0;
    float _currentHeight = 0;
    Size _allSize = Size::ZERO;
    AZStd::string _face = "";
    float _fontSize = 0;
    Color4B _color = Color4B::WHITE;
    bool _beginParser = false;
    bool _bold = false;
    int _letterSpacing;
};

NS_FGUI_END
