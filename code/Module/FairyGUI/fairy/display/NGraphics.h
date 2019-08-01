#pragma once

#include "FairyGUI/fairy/FairyGUIMacros.h"

NS_FGUI_BEGIN

class DisplayObject;

class NGraphics
{
public:
    NGraphics(DisplayObject* displayObj);
    ~NGraphics();

    void RefreshData();

    DisplayObject* GetTarget() { return m_target; }

private:

    DisplayObject* m_target;
};

NS_FGUI_END