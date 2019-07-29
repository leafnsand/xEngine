#include "Texture2D.h"

NS_FGUI_BEGIN

Texture2D::Texture2D():
    data(nullptr)
{
    
}

Texture2D* Texture2D::createEmpty()
{
    return nullptr;
}

NS_FGUI_END