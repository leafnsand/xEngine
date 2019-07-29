#pragma once


#include "ui/GScrollBar.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_scrollbar_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.scrollbar");
        return 1;
    }

    static int l_fairy_scrollbar_setScrollPane(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GScrollBar>(L, 1, "fairy.scrollbar");      
        const auto scrollpanel = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpanel");
        bool value = lua_toboolean(L, 3) == 1;
        if (self)
        {
            self->setScrollPane(scrollpanel, value);
        }
        return 0;
    }

    static int l_fairy_scrollbar_setDisplayPerc(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GScrollBar>(L, 1, "fairy.scrollbar");
        
        if (self)
        {
            auto value = luaL_checknumber(L, 2);
            self->SafeCast<GScrollBar>()->setDisplayPerc(value);
        }
        return 0;
    }

    static int l_fairy_scrollbar_setScrollPerc(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GScrollBar>(L, 1, "fairy.scrollbar");        
        if (self)
        {
            auto value = luaL_checknumber(L, 2);
            self->SafeCast<GScrollBar>()->setScrollPerc(value);
        }
        return 0;
    }

    static int l_fairy_scrollbar_getMinSize(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GScrollBar>(L, 1, "fairy.scrollbar");        
        if (self)
        {
            self->SafeCast<GScrollBar>()->getMinSize();
        }
        return 0;
    }

    static const luaL_Reg scrollbar_method[] =
    {
        { "__tostring", l_fairy_scrollbar_tostring },
        { "SetScrollPane", l_fairy_scrollbar_setScrollPane },
        { "SetDisplayPerc", l_fairy_scrollbar_setDisplayPerc },
        { "SetScrollPerc", l_fairy_scrollbar_setScrollPerc },
        { "GetMinSize", l_fairy_scrollbar_getMinSize },
    { nullptr, nullptr }
    };
}