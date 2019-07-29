#pragma once


#include "ui/GObjectPool.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_objectpool_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.objectpool");
        return 1;
    }

    static int l_fairy_objectpool_getObject(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObjectPool>(L, 1, "fairy.objectpool");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getObject(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_objectpool_returnObject(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObjectPool>(L, 1, "fairy.objectpool");
        const auto value = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (obj && value)
        {
            obj->returnObject(value);
        }
        return 0;
    }

    static const luaL_Reg objectpool_method[] =
    {
        { "__tostring", l_fairy_objectpool_tostring },
        { "GetObject", l_fairy_objectpool_getObject },
        { "ReturnObject", l_fairy_objectpool_returnObject },
        { nullptr, nullptr }
    };
}

