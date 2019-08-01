#include "BitMapFont.h"


NS_FGUI_BEGIN

BitmapFont* BitmapFont::create()
{
    return new BitmapFont();
}

void BitmapFont::ReleaseAtlas()
{
    fontAtlas.clear();
}

NS_FGUI_END