#pragma once


#include "ui/GTextField.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_basictextfield_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.basictextfield");
        return 1;
    }

    static int l_fairy_basictextfield_getText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");        
        if(obj)
        {
            lua_pushstring(L, obj->getText().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_basictextfield_setText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setText(value);
        }
        return 0;
    }

    static int l_fairy_basictextfield_getAutoSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");        
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getAutoSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_basictextfield_setAutoSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");
        const auto value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setAutoSize((TextAutoSize)value);
        }
        return 0;
    }

    static int l_fairy_basictextfield_isSingleLine(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");        
        if(obj)
        {
            lua_pushboolean(L, obj->isSingleLine());
            return 1;
        }
        return 0;
    }

    static int l_fairy_basictextfield_setSingleLine(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");
        const auto value = lua_toboolean(L, 2) == 1;        
        if(obj)
        {
            obj->setSingleLine(value);
        }
        return 0;
    }

    static int l_fairy_basictextfield_getTextFormat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");        
        if(obj)
        {
            n3lua_pushfairyclass(L, obj->getTextFormat());
            return 1;
        }
        return 0;
    }

    static int l_fairy_basictextfield_applyTextFormat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GBasicTextField>(L, 1, "fairy.basictextfield");        
        if(obj)
        {
            obj->applyTextFormat();
        }
        return 0;
    }

    static const luaL_Reg basictextfield_method[] =
    {
        { "__tostring", l_fairy_basictextfield_tostring },
        { "GetText", l_fairy_basictextfield_getText },
        { "SetText", l_fairy_basictextfield_setText },
        { "GetAutoSize", l_fairy_basictextfield_getAutoSize },
        { "SetAutoSize", l_fairy_basictextfield_setAutoSize },
        { "IsSingleLine", l_fairy_basictextfield_isSingleLine },
        { "SetSingleLine", l_fairy_basictextfield_setSingleLine },
        { "GetTextFormat", l_fairy_basictextfield_getTextFormat },
        { "ApplyTextFormat", l_fairy_basictextfield_applyTextFormat },
        { nullptr, nullptr }
    };
}
