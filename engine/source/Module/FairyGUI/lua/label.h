#pragma once

#include "ui/GLabel.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_label_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.label");
        return 1;
    }

    static int l_fairy_label_GetTitle(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            lua_pushstring(L, label->getTitle().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_label_SetTitle(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            const auto* title = luaL_checkstring(L, 2);
            label->setTitle(title);
        }
        return 0;
    }

    static int l_fairy_label_GetText(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            lua_pushstring(L, label->getText().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_label_SetText(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            const auto* text = luaL_checkstring(L, 2);
            label->setText(text);
        }
        return 0;
    }

    static int l_fairy_label_GetIcon(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            lua_pushstring(L, label->getIcon().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_label_SetIcon(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            const auto* icon = luaL_checkstring(L, 2);
            label->setIcon(icon);
        }
        return 0;
    }

    static int l_fairy_label_GetTitleColor(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            n3lua_pushfairycolor(L, label->getTitleColor());
            return 1;
        }
        return 0;
    }

    static int l_fairy_label_SetTitleColor(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            Color4B color;
            n3lua_checkfairycolor(L, 2, color);
            label->setTitleColor(color);
        }
        return 0;
    }

    static int l_fairy_label_GetTitleFontSize(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            lua_pushinteger(L, label->getTitleFontSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_label_SetTitleFontSize(lua_State* L)
    {
        const auto label = n3lua_checkfairyclass<GLabel>(L, 1, "fairy.label");        
        if (label)
        {
            const auto size = luaL_checkinteger(L, 2);
            label->setTitleFontSize(size);
        }
        return 0;
    }

    static const luaL_Reg label_method[] =
    {
        { "__tostring", l_fairy_label_tostring },
        { "GetTitle", l_fairy_label_GetTitle },
        { "SetTitle", l_fairy_label_SetTitle },
        { "GetText", l_fairy_label_GetText },
        { "SetText", l_fairy_label_SetText },
        { "GetIcon", l_fairy_label_GetIcon },
        { "SetIcon", l_fairy_label_SetIcon },
        { "GetTitleColor", l_fairy_label_GetTitleColor },
        { "SetTitleColor", l_fairy_label_SetTitleColor },
        { "GetTitleFontSize", l_fairy_label_GetTitleFontSize },
        { "SetTitleFontSize", l_fairy_label_SetTitleFontSize },
        { nullptr, nullptr }
    };
}