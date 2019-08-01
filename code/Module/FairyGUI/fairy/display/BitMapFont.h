#pragma once

#include "FairyGUIMacros.h"
#include "display/Texture2D.h"
#include "AzCore/std/containers/unordered_map.h"

NS_FGUI_BEGIN

struct FontLetterDefinition
{
    float offsetX;
    float offsetY;
    float U;
    float V;
    float width;
    float height;
    int xAdvance;
    bool validDefinition;
    Texture2D* texture;
};

class BitmapFont
{
public:
    static BitmapFont* create();
    
    void ReleaseAtlas();

    AZStd::unordered_map<int, FontLetterDefinition> fontAtlas;
};

NS_FGUI_END
