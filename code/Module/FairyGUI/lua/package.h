#pragma once


using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_package_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.package");
        return 1;
    }

    static int l_fairy_package_CreateObject(lua_State* L)
    {
        return 0;
    }

    static const luaL_Reg package_method[] =
    {
        { "__tostring", l_fairy_package_tostring },
        { "CreateObject", l_fairy_package_CreateObject },
        { nullptr, nullptr }
    };
}