#pragma once

#include "ui/GSlider.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_slider_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.slider");
        return 1;
    }

    static int l_fairy_slider_getTitleType(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GSlider>(L, 1, "fairy.slider");        
        if (self)
        {
            lua_pushinteger(L, (int)self->getTitleType());
            return 1;
        }
        return 0;
    }

    static int l_fairy_slider_setTitleType(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GSlider>(L, 1, "fairy.slider");        
        if (self)
        {
            auto value = luaL_checkinteger(L, 2);
            self->setTitleType((ProgressTitleType)value);
        }
        return 0;
    }

    static int l_fairy_slider_getMax(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GSlider>(L, 1, "fairy.slider");        
        if (self)
        {
            lua_pushnumber(L, self->getMax());
            return 1;
        }
        return 0;
    }

    static int l_fairy_slider_setMax(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GSlider>(L, 1, "fairy.slider");        
        if (self)
        {
            auto value = luaL_checknumber(L, 2);
            self->setMax(value);
        }
        return 0;
    }

    static int l_fairy_slider_getValue(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GSlider>(L, 1, "fairy.slider");        
        if (self)
        {
            lua_pushnumber(L, self->getValue());
            return 1;
        }
        return 0;
    }

    static int l_fairy_slider_setValue(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GSlider>(L, 1, "fairy.slider");        
        if (self)
        {
            auto value = luaL_checknumber(L, 2);
            self->setValue(value);
        }
        return 0;
    }

    static const luaL_Reg slider_method[] =
    {
        { "__tostring", l_fairy_slider_tostring },
        { "GetTitleType", l_fairy_slider_getTitleType },
        { "SetTitleType", l_fairy_slider_setTitleType },
        { "GetMax", l_fairy_slider_getMax },
        { "SetMax", l_fairy_slider_setMax },
        { "GetValue", l_fairy_slider_getValue },
        { "SetValue", l_fairy_slider_setValue },
    { nullptr, nullptr }
    };
}