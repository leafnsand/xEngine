#pragma once

#include "FairyGUI/EBus/FairyPanelComponentBus.h"
#include "FairyGUI/lua/util.h"
#include "FairyGUI/fairy/fairytypes.h"

namespace N3Lua
{
    static int l_uiPanel_tostring(lua_State* L)
    {
        lua_pushstring(L, "UIPanel");
        return 1;
    }

    static int l_uiPanel_SetConfigData(lua_State* L)
    {
        auto& id = n3lua_checkclass<AZ::EntityId>(L, 1, "UIPanel");
        const auto pkgname = luaL_checkstring(L, 2);
        const auto itemname = luaL_checkstring(L, 3);
        if (id.IsValid())
        {
            EBUS_EVENT_ID(id, Module::FairyPanelRequestBus, SetConfigData, pkgname, itemname);
        }
        return 0;
    }

    static int l_uiPanel_SetRenderMode(lua_State* L)
    {
        auto& id = n3lua_checkclass<AZ::EntityId>(L, 1, "UIPanel");
        auto value = luaL_checkinteger(L, 2);
        if (id.IsValid())
        {
            EBUS_EVENT_ID(id, Module::FairyPanelRequestBus, SetRenderMode, (fairygui::FairyRenderMode)value);
        }
        return 0;
    }

    static int l_uiPanel_SetSortingOrder(lua_State* L)
    {
        auto& id = n3lua_checkclass<AZ::EntityId>(L, 1, "UIPanel");
        auto value = luaL_checkinteger(L, 2);
        if (id.IsValid())
        {
            EBUS_EVENT_ID(id, Module::FairyPanelRequestBus, SetSortingOrder, value,true);
        }
        return 0;
    }

    static int l_uiPanel_CreateUI(lua_State* L)
    {
        auto& id = n3lua_checkclass<AZ::EntityId>(L, 1, "UIPanel");
        if (id.IsValid())
        {
            EBUS_EVENT_ID(id, Module::FairyPanelRequestBus, CreateUI);
        }
        return 0;
    }

    static int l_uiPanel_GetComponent(lua_State* L)
    {
        auto& id = n3lua_checkclass<AZ::EntityId>(L, 1, "UIPanel");
        if (id.IsValid())
        {
            GComponent* targetComponent = nullptr;
            EBUS_EVENT_ID_RESULT(targetComponent, id, Module::FairyPanelRequestBus, GetComponent);
            n3lua_pushfairyclass(L, targetComponent);
            return 1;
        }
        return 0;
    }

    static const luaL_Reg uiPanel_method[] = 
    {
        { "__tostring",         l_uiPanel_tostring },
        { "SetConfigData",      l_uiPanel_SetConfigData },
        { "SetRenderMode",      l_uiPanel_SetRenderMode },
        { "SetSortingOrder",    l_uiPanel_SetSortingOrder },
        { "CreateUI",           l_uiPanel_CreateUI },
        { "GetComponent",       l_uiPanel_GetComponent },
        { nullptr, nullptr }
    };
}
