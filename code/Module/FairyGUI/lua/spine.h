#pragma once

#include "FairyGUI/extension/GSpine.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_spine_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.spine");
        return 1;
    }

    static int l_fairy_spine_getspineid(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GSpine>(L, 1, "fairy.spine");        
        if (obj)
        {
            n3lua_pushclass(L, "spine", obj->GetSpineId());
            return 1;
        }
        */
        return 0;
    }

    static int l_fairy_spine_Init(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GSpine>(L, 1, "fairy.spine");
        const auto value = luaL_checkstring(L, 2);        
        if (obj)
        {
            obj->Init(value);
        }
        return 0;
    }

    static int l_fairy_spine_RefreshTransform(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GSpine>(L, 1, "fairy.spine");        
        if (obj)
        {
            obj->RefreshTransform();
        }
        return 0;
    }

    static const luaL_Reg spine_method[] =
    {
        { "__tostring", l_fairy_spine_tostring },
        { "Init", l_fairy_spine_Init },
        { "RefreshTransform", l_fairy_spine_RefreshTransform },
        { "GetSpineObj", l_fairy_spine_getspineid },
        { nullptr, nullptr }
    };
}