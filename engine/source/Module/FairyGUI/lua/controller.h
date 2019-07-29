#pragma once


#include "ui/Controller.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_controller_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.controller");
        return 1;
    }

    static int l_fairy_controller_getParent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getParent());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_setParent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = n3lua_checkfairyclass<GComponent>(L, 2, "fairy.component");
        if (obj && value)
        {
            obj->setParent(value);
        }
        return 0;
    }

    static int l_fairy_controller_getSelectedIndex(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushinteger(L, obj->getSelectedIndex());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_setSelectedIndex(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkinteger(L, 2); 
        if (obj)
        {
            obj->setSelectedIndex(value);
        }
        return 0;
    }

    static int l_fairy_controller_getSelectedPage(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushstring(L, obj->getSelectedPage().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_setSelectedPage(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            obj->setSelectedPage(value);
        }
        return 0;
    }

    static int l_fairy_controller_getSelectedPageId(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushstring(L, obj->getSelectedPageId().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_setSelectedPageId(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            obj->setSelectedPageId(value);
        }
        return 0;
    }

    static int l_fairy_controller_getPrevisousIndex(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushinteger(L, obj->getPrevisousIndex());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_getPreviousPage(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushstring(L, obj->getPreviousPage().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_getPreviousPageId(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushstring(L, obj->getPreviousPageId().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_getPageCount(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            lua_pushinteger(L, obj->getPageCount());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_hasPage(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            lua_pushboolean(L, obj->hasPage(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_getPageIndexById(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            lua_pushinteger(L, obj->getPageIndexById(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_getPageNameById(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            lua_pushstring(L, obj->getPageNameById(value).c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_getPageId(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            lua_pushstring(L, obj->getPageId(value).c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_controller_setOppositePageId(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        const auto value = luaL_checkstring(L, 2);
        if (obj)
        {
            obj->setOppositePageId(value);
        }
        return 0;
    }

    static int l_fairy_controller_runActions(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GController>(L, 1, "fairy.controller");
        if (obj)
        {
            obj->runActions();
        }
        return 0;
    }

    static const luaL_Reg controller_method[] =
    {
        { "__tostring", l_fairy_controller_tostring },
        { "GetParent", l_fairy_controller_getParent },
        { "SetParent", l_fairy_controller_setParent },
        { "GetSelectedIndex", l_fairy_controller_getSelectedIndex },
        { "SetSelectedIndex", l_fairy_controller_setSelectedIndex },
        { "GetSelectedPage", l_fairy_controller_getSelectedPage },
        { "SetSelectedPage", l_fairy_controller_setSelectedPage },
        { "GetSelectedPageId", l_fairy_controller_getSelectedPageId },
        { "SetSelectedPageId", l_fairy_controller_setSelectedPageId },
        { "GetPrevisousIndex", l_fairy_controller_getPrevisousIndex },
        { "GetPreviousPage", l_fairy_controller_getPreviousPage },
        { "GetPreviousPageId", l_fairy_controller_getPreviousPageId },
        { "GetPageCount", l_fairy_controller_getPageCount },
        { "HasPage", l_fairy_controller_hasPage },
        { "GetPageIndexById", l_fairy_controller_getPageIndexById },
        { "GetPageNameById", l_fairy_controller_getPageNameById },
        { "GetPageId", l_fairy_controller_getPageId },
        { "SetOppositePageId", l_fairy_controller_setOppositePageId },
        { "RunActions", l_fairy_controller_runActions },
        { nullptr, nullptr }
    };
}