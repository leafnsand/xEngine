#pragma once

#include "ui/PopupMenu.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_popupmenu_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.popupmenu");
        return 1;
    }

    static int l_fairy_popupmenu_addItem(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            LuaRefCallBack callback(L, 3);
            obj->addItem(value, [callback](EventContext* context)
            {
                callback.call();
            });
        }
        return 0;
    }

    static int l_fairy_popupmenu_addItemAt(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        const auto index = luaL_checkinteger(L, 3);
        if (obj)
        {
            LuaRefCallBack callback(L, 4);
            obj->addItemAt(value, index, [callback](EventContext* context)
            {
                callback.call();
            });
        }
        return 0;
    }

    static int l_fairy_popupmenu_addSeperator(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        if (obj)
        {
            obj->addSeperator();
        }        
        return 0;
    }

    static int l_fairy_popupmenu_getItemName(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            lua_pushstring(L ,obj->getItemName(value).c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_popupmenu_setItemText(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        const auto check = luaL_checkstring(L, 3);
        if (obj)
        {
            obj->setItemText(value, check);
        }
        return 0;
    }

    static int l_fairy_popupmenu_setItemVisible(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        const auto check = luaL_checkinteger(L, 3);
        if (obj)
        {
            obj->setItemVisible(value, check);
        }
        return 0;
    }

    static int l_fairy_popupmenu_setItemGrayed(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        const auto check = luaL_checkinteger(L, 3);
        if (obj)
        {
            obj->setItemGrayed(value, check);
        }
        return 0;
    }

    static int l_fairy_popupmenu_setItemCheckable(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        const auto check = luaL_checkinteger(L, 3);
        if (obj)
        {
            obj->setItemCheckable(value, check);
        }
        return 0;
    }

    static int l_fairy_popupmenu_setItemChecked(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        const auto check = luaL_checkinteger(L, 3);
        if (obj)
        {
            obj->setItemChecked(value, check);
        }
        return 0;
    }

    static int l_fairy_popupmenu_isItemChecked(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
           lua_pushboolean(L, obj->isItemChecked(value));
           return 1;
        }
        return 0;
    }

    static int l_fairy_popupmenu_removeItem(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            obj->removeItem(value);
        }
        return 0;
    }

    static int l_fairy_popupmenu_clearItems(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        if (obj)
        {
            obj->clearItems();
        }
        return 0;
    }

    static int l_fairy_popupmenu_getItemCount(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        if (obj)
        {
            lua_pushinteger(L, obj->getItemCount());
            return 1;
        }
        return 0;
    }

    static int l_fairy_popupmenu_getContentPane(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getContentPane());
            return 1;
        }
        return 0;
    }

    static int l_fairy_popupmenu_getList(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getList());
            return 1;
        }
        return 0;
    }

    static int l_fairy_popupmenu_show(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<PopupMenu>(L, 1, "fairy.popupmenu"); 
        if (obj)
        {
            if(argc == 1)
            {
                obj->show();
            }
            else if(argc == 3)
            {
                const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
                const auto value = luaL_checkinteger(L, 3);
            
                obj->show(target, (PopupDirection)value);
            }
        }
        return 0;
    }

    static const luaL_Reg popupmenu_method[] =
    {
        { "__tostring", l_fairy_popupmenu_tostring },
        { "AddItem", l_fairy_popupmenu_addItem },
        { "AddItemAt", l_fairy_popupmenu_addItemAt },
        { "AddSeperator", l_fairy_popupmenu_addSeperator },
        { "GetItemName", l_fairy_popupmenu_getItemName },
        { "SetItemText", l_fairy_popupmenu_setItemText },
        { "SetItemVisible", l_fairy_popupmenu_setItemVisible },
        { "SetItemGrayed", l_fairy_popupmenu_setItemGrayed },
        { "SetItemCheckable", l_fairy_popupmenu_setItemCheckable },
        { "SetItemChecked", l_fairy_popupmenu_setItemChecked },
        { "IsItemChecked", l_fairy_popupmenu_isItemChecked },
        { "RemoveItem", l_fairy_popupmenu_removeItem },
        { "ClearItems", l_fairy_popupmenu_clearItems },
        { "GetItemCount", l_fairy_popupmenu_getItemCount },
        { "GetContentPane", l_fairy_popupmenu_getContentPane },
        { "GetList", l_fairy_popupmenu_getList },
        { "Show", l_fairy_popupmenu_show },
        { nullptr, nullptr }
    };
}

