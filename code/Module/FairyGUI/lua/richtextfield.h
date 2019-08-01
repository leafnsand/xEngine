#pragma once


#include "ui/GRichTextField.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_richtextfield_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.richtextfield");
        return 1;
    }

    static int l_fairy_richtextfield_getText(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            lua_pushstring(L, self->getText().data());
            return 1;
        }
        return 0;
    }

    static int l_fairy_richtextfield_setText(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            const auto value = luaL_checkstring(L, 2);
            self->setText(value);
        }
        return 0;
    }

    static int l_fairy_richtextfield_getAutoSize(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            lua_pushinteger(L, (int)self->getAutoSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_richtextfield_setAutoSize(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            const auto value = luaL_checkinteger(L, 2);
            self->setAutoSize((TextAutoSize)value);
        }
        return 0;
    }

    static int l_fairy_richtextfield_isSingleLine(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            lua_pushboolean(L, self->isSingleLine());
            return 1;
        }
        return 0;
    }

    static int l_fairy_richtextfield_setSingleLine(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            const auto value = lua_toboolean(L, 2) == 1;
            self->setSingleLine(value);
        }
        return 0;
    }

    static int l_fairy_richtextfield_getTextFormat(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");        
        if (self)
        {
            n3lua_pushfairyclass(L, self->getTextFormat());
            return 1;
        }
        return 0;
    }

    static int l_fairy_richtextfield_applyTextFormat(lua_State* L)
    {
        const auto self = n3lua_checkfairyclass<GRichTextField>(L, 1, "fairy.richtextfield");
        
        if (self)
        {
           self->applyTextFormat();
        }
        return 0;
    }


    static const luaL_Reg richtextfield_method[] =
    {
        { "__tostring", l_fairy_richtextfield_tostring },
        { "GetText", l_fairy_richtextfield_getText },
        { "SetText", l_fairy_richtextfield_setText },
        { "GetAutoSize", l_fairy_richtextfield_getAutoSize },
        { "SetAutoSize", l_fairy_richtextfield_setAutoSize },
        { "IsSingleLine", l_fairy_richtextfield_isSingleLine },
        { "SetSingleLine", l_fairy_richtextfield_setSingleLine },
        { "GetTextFormat", l_fairy_richtextfield_getTextFormat },
        { "ApplyTextFormat", l_fairy_richtextfield_applyTextFormat },
        { nullptr, nullptr }
    };
}