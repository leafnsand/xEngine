#pragma once


#include "ui/GGroup.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_group_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.group");
        return 1;
    }

    static int l_fairy_group_getLayout(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");        
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getLayout());
            return 1;
        }
        return 0;
    }

    static int l_fairy_group_setLayout(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");
        const auto value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setLayout((GroupLayoutType)value);
        }
        return 0;
    }

    static int l_fairy_group_getColumnGap(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");        
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getColumnGap());
            return 1;
        }
        return 0;
    }

    static int l_fairy_group_setColumnGap(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");
        const auto value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setColumnGap(value);
        }
        return 0;
    }

    static int l_fairy_group_getLineGap(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");        
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getLineGap());
            return 1;
        }
        return 0;
    }

    static int l_fairy_group_setLineGap(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");
        const auto value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setLineGap(value);
        }
        return 0;
    }

    static int l_fairy_group_setBoundsChangedFlag(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");
        const auto value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setBoundsChangedFlag(value);
        }
        return 0;
    }

    static int l_fairy_group_moveChildren(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);        
        if (obj)
        {
            obj->moveChildren(valueX, valueY);
        }
        return 0;
    }

    static int l_fairy_group_resizeChildren(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GGroup>(L, 1, "fairy.group");
        const auto valueW = luaL_checknumber(L, 2);
        const auto valueH = luaL_checknumber(L, 3);        
        if (obj)
        {
            obj->resizeChildren(valueW, valueH);
        }
        return 0;
    }

    static const luaL_Reg group_method[] =
    {
        { "__tostring", l_fairy_group_tostring },
        { "GetLayout", l_fairy_group_getLayout },
        { "SetLayout", l_fairy_group_setLayout },
        { "GetColumnGap", l_fairy_group_getColumnGap },
        { "SetColumnGap", l_fairy_group_setColumnGap },
        { "GetLineGap", l_fairy_group_getLineGap },
        { "SetLineGap", l_fairy_group_setLineGap },
        { "SetBoundsChangedFlag", l_fairy_group_setBoundsChangedFlag },
        { "MoveChildren", l_fairy_group_moveChildren },
        { "ResizeChildren", l_fairy_group_resizeChildren },
        { nullptr, nullptr }
    };
}