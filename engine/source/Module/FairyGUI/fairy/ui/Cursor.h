#pragma once

#include "FairyGUIMacros.h"
#include "GGraph.h"

NS_FGUI_BEGIN

class Cursor : public GGraph
{
    FUI_RTTI_DERIVED(Cursor, GGraph)
public:
    CREATE_FUNC(Cursor);

    void SetParentNode(GObject* obj);
    void DrawCursor();
    AZ::Transform GetGlobalMatrix44() override;
    void SetOffset(Vec2 offset);

private:
    bool init() { return true; }

    GObject* _parentNode = nullptr;
    Vec2 _offset = Vec2::ZERO;
};

NS_FGUI_END
