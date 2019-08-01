#pragma once

#include "ui/GTextField.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_textfield_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.textfield");
        return 1;
    }

    static int l_fairy_textfield_IsUBBEnabled(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            lua_pushboolean(L, obj->isUBBEnabled());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetUBBEnabled(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        bool value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setUBBEnabled(value);
        }
        return 0;
    }

    static int l_fairy_textfield_GetAutoSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            lua_pushinteger(L, int(obj->getAutoSize()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetAutoSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setAutoSize(TextAutoSize(value));
        }
        return 0;
    }

    static int l_fairy_textfield_IsSingleLine(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            lua_pushboolean(L, obj->isSingleLine());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetSingleLine(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        bool value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setSingleLine(value);
        }
        return 0;
    }

    static int l_fairy_textfield_GetTextFormat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getTextFormat());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_ApplyTextFormat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            obj->applyTextFormat();
        }
        return 0;

    }

    static int l_fairy_textfield_GetTextSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            n3lua_pushfairysize(L, obj->getTextSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_GetColor(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            n3lua_pushfairycolor(L, obj->getColor());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetColor(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        Color4B color;
        n3lua_checkfairycolor(L, 2, color);
        if (obj)
        {
            obj->setColor(color);
        }
        return 0;
    }

    static int l_fairy_textfield_GetFontSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            lua_pushnumber(L, obj->getFontSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetFontSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        float value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setFontSize(value);
        }
        return 0;
    }

    static int l_fairy_textfield_GetDimensions(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            n3lua_pushfairysize(L, obj->getDimensions());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetDimensions(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        float valueX = luaL_checknumber(L, 2);
        float valueY = luaL_checknumber(L, 3);
        if (obj)
        {
            obj->setDimensions(valueX, valueY);
        }
        return 0;
    }

    static int l_fairy_textfield_GetOverflow(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getOverflow());
            return 1;
        }
        return 0;
    }

    static int l_fairy_textfield_SetOverflow(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GTextField>(L, 1, "fairy.textfield");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setOverflow((Overflow)value);
        }
        return 0;        
    }

    static const luaL_Reg textfield_method[] =
    {
        { "__tostring", l_fairy_textfield_tostring },
        { "IsUBBEnabled", l_fairy_textfield_IsUBBEnabled },
        { "SetUBBEnabled", l_fairy_textfield_SetUBBEnabled },
        { "GetAutoSize", l_fairy_textfield_GetAutoSize },
        { "SetAutoSize", l_fairy_textfield_SetAutoSize },
        { "IsSingleLine", l_fairy_textfield_IsSingleLine },
        { "SetSingleLine", l_fairy_textfield_SetSingleLine },
        { "GetTextFormat", l_fairy_textfield_GetTextFormat },
        { "ApplyTextFormat", l_fairy_textfield_ApplyTextFormat },
        { "GetTextSize", l_fairy_textfield_GetTextSize },
        { "GetColor", l_fairy_textfield_GetColor },
        { "SetColor", l_fairy_textfield_SetColor },
        { "GetFontSize", l_fairy_textfield_GetFontSize },
        { "SetFontSize", l_fairy_textfield_SetFontSize },
        { "GetDimensions", l_fairy_textfield_GetDimensions },
        { "SetDimensions", l_fairy_textfield_SetDimensions },
        { "GetOverflow", l_fairy_textfield_GetOverflow },
        { "SetOverflow", l_fairy_textfield_SetOverflow },
        { nullptr, nullptr }
    };
}