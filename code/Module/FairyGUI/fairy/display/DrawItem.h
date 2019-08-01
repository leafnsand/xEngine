#pragma once

#include "FairyGUI/fairy/FairyGUIMacros.h"
#include "FairyGUI/fairy/display/DisplayObject.h"

NS_FGUI_BEGIN

class DrawItem : public DisplayObject 
{
public:
    DrawItem(const AZStd::string& entityName, GObject* target);
    ~DrawItem();
};

NS_FGUI_END