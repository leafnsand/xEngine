#pragma once

#include "ui/GTextInput.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_textinput_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.textinput");
        return 1;
    }


    static int l_fairy_textinput_GetText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        if (obj)
        {
            lua_pushstring(L, obj->getText().c_str());
            return 1;
        }
        return 0;         
    }

    static int l_fairy_textinput_setText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        const auto value = luaL_checkstring(L, 2);        
        if (obj)
        {
            obj->setText(value);
        
        }
        return 0;
    }

    static int l_fairy_textinput_IsSingleLine(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");        
        if (obj)
        {
            lua_pushboolean(L, obj->isSingleLine());
            return 1;

        }
        return 0;
    }

    static int l_fairy_textinput_SetSingleLine(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        bool value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setSingleLine(value);
        }
        return 0;
    }

    static int l_fairy_textinput_GetTextFormat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getTextFormat());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textinput_ApplyTextFormat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");        
        if (obj)
        {
            obj->applyTextFormat();
        }
        return 0;
    }

    static int l_fairy_textinput_SetPrompt(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        const AZStd::string value = luaL_checkstring(L, 2);        
        if (obj)
        {
            obj->setPrompt(value);
        }
        return 0;
    }

    static int l_fairy_textinput_SetPassword(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        bool value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setPassword(value);
        }
        return 0;
    }

    static int l_fairy_textinput_SetKeyboardType(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        bool value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setKeyboardType(value);
        }
        return 0;
    }

    static int l_fairy_textinput_SetMaxLength(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        bool value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setMaxLength(value);
        }
        return 0;
    }

    static int l_fairy_textinput_SetRestrict(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextInput>(L, 1, "fairy.textinput");
        const AZStd::string value = luaL_checkstring(L, 2);        
        if (obj)
        {
            obj->setRestrict(value);
        }
        return 0;
    }

    static const luaL_Reg textinput_method[] =
    {
        { "__tostring", l_fairy_textinput_tostring },
        { "GetText", l_fairy_textinput_GetText },
        { "SetText", l_fairy_textinput_setText },
        { "IsSingleLine", l_fairy_textinput_IsSingleLine },
        { "SetSingleLine", l_fairy_textinput_SetSingleLine },
        { "GetTextFormat", l_fairy_textinput_GetTextFormat },
        { "ApplyTextFormat", l_fairy_textinput_ApplyTextFormat },
        { "SetPrompt", l_fairy_textinput_SetPrompt },
        { "SetPassword", l_fairy_textinput_SetPassword },
        { "SetKeyboardType", l_fairy_textinput_SetKeyboardType },
        { "SetMaxLength", l_fairy_textinput_SetMaxLength },
        { "SetRestrict", l_fairy_textinput_SetRestrict },
        { nullptr, nullptr }
    };
}