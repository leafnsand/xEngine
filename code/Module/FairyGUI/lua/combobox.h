#pragma once


#include "ui/GComboBox.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_combobox_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.combobox");
        return 1;
    }

    static int l_fairy_combobox_getTitle(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if(obj)
        {
            lua_pushstring(L, obj->getTitle().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_setTitle(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setTitle(value);
        }
        return 0;
    }

    static int l_fairy_combobox_getText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if(obj)
        {
            lua_pushstring(L, obj->getText().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_setText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setText(value);
        }
        return 0;
    }

    static int l_fairy_combobox_getIcon(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if(obj)
        {
            lua_pushstring(L, obj->getIcon().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_setIcon(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setIcon(value);
        }
        return 0;
    }

    static int l_fairy_combobox_getValue(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if(obj)
        {
            lua_pushstring(L, obj->getValue().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_setValue(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");
        const auto value = luaL_checkstring(L, 2);        
        if(obj)
        {
            obj->setValue(value);
        }
        return 0;
    }

    static int l_fairy_combobox_getSelectedIndex(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if(obj)
        {
            lua_pushinteger(L, obj->getSelectedIndex());
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_setSelectedIndex(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");
        const auto value = luaL_checkinteger(L, 2);        
        if(obj)
        {
            obj->setSelectedIndex(value);
        }
        return 0;
    }

    static int l_fairy_combobox_getSelectionController(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getSelectionController());
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_setSelectionController(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");        
        if (obj && controller)
        {
            obj->setSelectionController(controller);
        }
        return 0;
    }

    static int l_fairy_combobox_getItems(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if (obj)
        {
            AZStd::vector<AZStd::string> result = obj->getItems();
            lua_createtable(L, int(result.size()), 0);
            for (int i = 0; i < result.size(); ++i)
            {
                lua_pushstring(L, result[i].c_str());
                lua_rawseti(L, -2, i + 1);
            }
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_getIcons(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if (obj)
        {
            AZStd::vector<AZStd::string> result = obj->getIcons();
            lua_createtable(L, int(result.size()), 0);
            for (int i = 0; i < result.size(); ++i)
            {
                lua_pushstring(L, result[i].c_str());
                lua_rawseti(L, -2, i + 1);
            }
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_getValues(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if (obj)
        {
            AZStd::vector<AZStd::string> result = obj->getValues();
            lua_createtable(L, int(result.size()), 0);
            for (int i = 0; i < result.size(); ++i)
            {
                lua_pushstring(L, result[i].c_str());
                lua_rawseti(L, -2, i + 1);
            }
            return 1;
        }
        return 0;
    }

    static int l_fairy_combobox_refresh(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GComboBox>(L, 1, "fairy.combobox");        
        if(obj)
        {
            obj->refresh();
        }
        return 0;
    }

    static const luaL_Reg combobox_method[] =
    {
        { "__tostring", l_fairy_combobox_tostring },
        { "GetTitle", l_fairy_combobox_getTitle },
        { "SetTitle", l_fairy_combobox_setTitle },
        { "GetText", l_fairy_combobox_getText },
        { "SetText", l_fairy_combobox_setText },
        { "GetIcon", l_fairy_combobox_getIcon },
        { "SetIcon", l_fairy_combobox_setIcon },
        { "GetValue", l_fairy_combobox_getValue },
        { "SetValue", l_fairy_combobox_setValue },
        { "GetSelectedIndex", l_fairy_combobox_getSelectedIndex },
        { "SetSelectedIndex", l_fairy_combobox_setSelectedIndex },
        { "GetSelectionController", l_fairy_combobox_getSelectionController },
        { "SetSelectionController", l_fairy_combobox_setSelectionController },
        { "GetItems", l_fairy_combobox_getItems },
        { "GetIcons", l_fairy_combobox_getIcons },
        { "GetValues", l_fairy_combobox_getValues },
        { "Refresh", l_fairy_combobox_refresh },
        { nullptr, nullptr }
    };
}
