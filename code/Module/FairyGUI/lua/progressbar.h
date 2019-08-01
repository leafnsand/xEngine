#pragma once

#include "ui/GProgressBar.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_progressbar_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.progressbar");
        return 1;
    }

    static int l_fairy_progressbar_getTitleType(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            lua_pushinteger(L, (int)self->getTitleType());
            return 1;
        }
        return 0;
    }

    static int l_fairy_progressbar_setTitleType(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            const auto value = luaL_checkinteger(L, 2);
            self->setTitleType((ProgressTitleType)value);
        }
        return 0;
    }

    static int l_fairy_progressbar_getMax(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            lua_pushnumber(L, self->getMax());
            return 1;
        }
        return 0;
    }

    static int l_fairy_progressbar_setMax(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            const auto value = luaL_checknumber(L, 2);
            self->setMax(value);
        }
        return 0;
    }

    static int l_fairy_progressbar_getValue(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            lua_pushnumber(L, self->getValue());
            return 1;
        }
        return 0;
    }

    static int l_fairy_progressbar_setValue(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            const auto value = luaL_checknumber(L, 2);
            self->setValue(value);
        }
        return 0;
    }

    static int l_fairy_progressbar_tweenValue(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GProgressBar>(L, 1, "fairy.progressbar");        
        if (self)
        {
            const auto value = luaL_checknumber(L, 2);
            const auto duration = luaL_checknumber(L, 3);
            self->tweenValue(value, (float)duration);
        }
        return 0;
    }

    static const luaL_Reg progressbar_method[] =
    {
        { "__tostring", l_fairy_progressbar_tostring },
        { "GetTitleType", l_fairy_progressbar_getTitleType },
        { "SetTitleType", l_fairy_progressbar_setTitleType },
        { "GetMax", l_fairy_progressbar_getMax },
        { "SetMax", l_fairy_progressbar_setMax },
        { "GetValue", l_fairy_progressbar_getValue },
        { "SetValue", l_fairy_progressbar_setValue },
        { "TweenValue", l_fairy_progressbar_tweenValue },
        { nullptr, nullptr }
    };
}