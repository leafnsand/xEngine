#pragma once

#include "ui/GGraph.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_graph_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.graph");
        return 1;
    }

    static int l_fairy_graph_drawRect(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGraph>(L, 1, "fairy.graph");
        Color4B color, fillColor;
        const auto w = luaL_checknumber(L, 2);
        const auto h = luaL_checknumber(L, 3);
        const auto lineSize = luaL_checkinteger(L, 4);
        n3lua_checkfairycolor(L, 5, color);
        n3lua_checkfairycolor(L, 6, fillColor);        
        if (obj)
        {
            obj->drawRect(w, h, lineSize, Color4F(color), Color4F(fillColor));
        }
        return 0;
    }

    static int l_fairy_graph_drawEllipse(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGraph>(L, 1, "fairy.graph");
        Color4B color, fillColor;
        const auto w = luaL_checknumber(L, 2);
        const auto h = luaL_checknumber(L, 3);
        const auto lineSize = luaL_checkinteger(L, 4);
        n3lua_checkfairycolor(L, 5, color);
        n3lua_checkfairycolor(L, 6, fillColor);        
        if (obj)
        {
            obj->drawEllipse(w, h, lineSize, Color4F(color), Color4F(fillColor));
        }
        return 0;
    }

    static int l_fairy_graph_isEmpty(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGraph>(L, 1, "fairy.graph");        
        if (obj)
        {
            lua_pushboolean(L, obj->isEmpty());
            return 1;
        }
        return 0;
    }


    static const luaL_Reg graph_method[] =
    {
        { "__tostring", l_fairy_graph_tostring },
        { "DrawRect", l_fairy_graph_drawRect },
        { "DrawEllipse", l_fairy_graph_drawEllipse },
        { "IsEmpty", l_fairy_graph_isEmpty },
        { nullptr, nullptr }
    };
}