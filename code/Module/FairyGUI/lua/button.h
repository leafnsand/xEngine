#pragma once

#include "ui/GButton.h"
#include "util.h"


using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_button_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.button");
        return 1;
    }

    static int l_fairy_button_getTitle(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushstring(L, obj->getTitle().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setTitle(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setTitle(value);
        }
        return 0;
    }

    static int l_fairy_button_getText(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushstring(L, obj->getText().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setText(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setText(value);
        }
        return 0;
    }

    static int l_fairy_button_getIcon(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushstring(L, obj->getIcon().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setIcon(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setIcon(value);
        }
        return 0;
    }

    static int l_fairy_button_getSelectedTitle(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushstring(L, obj->getSelectedTitle().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setSelectedTitle(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setSelectedTitle(value);
        }
        return 0;
    }

    static int l_fairy_button_getSelectedIcon(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushstring(L, obj->getSelectedIcon().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setSelectedIcon(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setSelectedIcon(value);
        }
        return 0;
    }

    static int l_fairy_button_getTitleColor(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if (obj)
        {
            n3lua_pushfairycolor(L, obj->getTitleColor());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setTitleColor(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        Color4B color;
        n3lua_checkfairycolor(L, 2, color);        
        if (obj)
        {
            obj->setTitleColor(color);
        }
        return 0;
    }

    static int l_fairy_button_getTitleFontSize(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushinteger(L, obj->getTitleFontSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setTitleFontSize(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = luaL_checkinteger(L, 2);        
        if (obj)
        {
            obj->setTitleFontSize(value);
        }
        return 0;
    }

    static int l_fairy_button_isSelected(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushboolean(L, obj->isSelected());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setSelected(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setSelected(value);
        }
        return 0;
    }

    static int l_fairy_button_getRelatedController(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getRelatedController());
            return 1;
        }
        return 0;
    }
    
    static int l_fairy_button_setRelatedController(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");
        if (obj && controller)
        {
            obj->setRelatedController(controller);
        }
        return 0;
    }

    static int l_fairy_button_isChangeStateOnClick(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");        
        if(obj)
        {
            lua_pushboolean(L, obj->isChangeStateOnClick());
            return 1;
        }
        return 0;
    }

    static int l_fairy_button_setChangeStateOnClick(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GButton>(L, 1, "fairy.button");
        const auto value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setChangeStateOnClick(value);
        }
        return 0;
    }

    static const luaL_Reg button_method[] =
    {
        { "__tostring", l_fairy_button_tostring },
        { "GetTitle", l_fairy_button_getTitle },
        { "SetTitle", l_fairy_button_setTitle },
        { "GetText", l_fairy_button_getText },
        { "SetText", l_fairy_button_setText },
        { "GetIcon", l_fairy_button_getIcon },
        { "SetIcon", l_fairy_button_setIcon },
        { "GetSelectedTitle", l_fairy_button_getSelectedTitle },
        { "SetSelectedTitle", l_fairy_button_setSelectedTitle },
        { "GetSelectedIcon", l_fairy_button_getSelectedIcon },
        { "SetSelectedIcon", l_fairy_button_setSelectedIcon },
        { "GetTitleColor", l_fairy_button_getTitleColor },
        { "SetTitleColor", l_fairy_button_setTitleColor },
        { "GetTitleFontSize", l_fairy_button_getTitleFontSize },
        { "SetTitleFontSize", l_fairy_button_setTitleFontSize },
        { "IsSelected", l_fairy_button_isSelected },
        { "SetSelected", l_fairy_button_setSelected },
        { "GetRelatedController", l_fairy_button_getRelatedController },
        { "SetRelatedController", l_fairy_button_setRelatedController },
        { "IsChangeStateOnClick", l_fairy_button_isChangeStateOnClick },
        { "SetChangeStateOnClick", l_fairy_button_setChangeStateOnClick },
        { nullptr, nullptr }
    };
}

