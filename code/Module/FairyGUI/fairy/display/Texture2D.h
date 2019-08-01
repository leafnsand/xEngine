#pragma once

#include "FairyGUI/fairy/FairyGUIMacros.h"
#include <AzCore/std/string/string.h>


NS_FGUI_BEGIN

class Texture2D
{
public:
    Texture2D();

    static Texture2D* create(AZStd::string& path);
    static Texture2D* createEmpty();

    ~Texture2D();

    int getWidth();
    int getHeight();

    void* data = nullptr;
    bool isFont = false;
};

NS_FGUI_END
