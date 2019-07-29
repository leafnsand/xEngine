#include "FonsFont.h"
#include "SpriteFrame.h"


NS_FGUI_BEGIN
FUI__Implement_Singleton(FonsFont)

bool FonsFont::IsFontSpriteFrame(SpriteFrame* sf)
{
    for (auto element : spriteFrameMap)
    {
        if (element.second == sf)
            return true;
    }
    return false;
}

SpriteFrame* FonsFont::GetTextSpriteFrame(AZStd::string& fontName)
{
    auto it = spriteFrameMap.find(fontName);
    if (it != spriteFrameMap.end())
        return it->second;

    Texture2D* ptex = GetTextTexture(fontName);
    SpriteFrame* psf = new SpriteFrame;
    psf->texture = ptex;
    spriteFrameMap.insert(make_pair(fontName, psf));
    return psf;
}

void FonsFont::ReleaseMap()
{
    for (auto element : spriteFrameMap)
    {
        delete element.second;
    }
    spriteFrameMap.clear();
}
NS_FGUI_END