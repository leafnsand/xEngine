#pragma once

#include "FairyGUIMacros.h"
#include <AzCore/std/containers/unordered_map.h>
#include "FairyGUI.h"
#include "fairytypes.h"

NS_FGUI_BEGIN

class FonsFont
{
    FUI_Declare_Singleton(FonsFont)
public:
    static bool Decutf8(unsigned int& utf8state, unsigned int& codepoint, const char* text);
    static Size GetTextBounds(AZStd::string fontName, float fontSize, const char* text, const char* end = nullptr);
    static void CaculateFont(AZStd::string fontName, Vec2 beginPos, int fontSize, const char* text, AZStd::vector<Vec2>& fontVertex);
    static int GetFonsFontIndex(const char* face);

    Texture2D* GetTextTexture(AZStd::string& fontName);

    bool IsFontSpriteFrame(SpriteFrame* sf);

    SpriteFrame* GetTextSpriteFrame(AZStd::string& fontName);

    void ReleaseMap();

private:
    AZStd::unordered_map<AZStd::string, SpriteFrame*> spriteFrameMap;
};

NS_FGUI_END
