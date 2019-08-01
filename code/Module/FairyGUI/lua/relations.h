#pragma once

#include "ui/Relations.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_relations_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.relations");
        return 1;
    }

    static int l_fairy_relations_add(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto type = luaL_checkinteger(L, 3);
        const auto isPer = (argc == 4) ? luaL_checkinteger(L, 4) : false;
        if (obj && target)
        {
            obj->add(target, (RelationType)type, isPer);
        }
        return 0;
    }

    static int l_fairy_relations_addItems(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto str = luaL_checkstring(L, 3);
        if(obj && target)
        {
            obj->addItems(target, str);
        }
        return 0;
    }

    static int l_fairy_relations_remove(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto type = luaL_checkinteger(L, 3);
        if (obj && target)
        {
            obj->remove(target, (RelationType)type);
        }
        return 0;
    }

    static int l_fairy_relations_contains(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (obj && target)
        {
            lua_pushboolean(L, obj->contains(target));
            return 1;
        }
        return 0;
    }

    static int l_fairy_relations_clearFor(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (obj && target)
        {
            obj->clearFor(target);
        }
        return 0;
    }

    static int l_fairy_relations_clearAll(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        if (obj)
        {
            obj->clearAll();
        }
        return 0;
    }

    static int l_fairy_relations_copyFrom(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        const auto target = n3lua_checkfairyclass<Relations>(L, 2, "fairy.relations");
        if (obj && target)
        {
            obj->copyFrom(*target);
        }
        return 0;
    }

    static int l_fairy_relations_isEmpty(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Relations>(L, 1, "fairy.relations");
        if (obj)
        {
            lua_pushboolean(L, obj->isEmpty());
            return 1;
        }
        return 0;
    }

    static const luaL_Reg relations_method[] =
    {
        { "__tostring", l_fairy_relations_tostring },
        { "Add", l_fairy_relations_add },
        { "AddItems", l_fairy_relations_addItems },
        { "Remove", l_fairy_relations_remove },
        { "Contains", l_fairy_relations_contains },
        { "ClearFor", l_fairy_relations_clearFor },
        { "ClearAll", l_fairy_relations_clearAll },
        { "CopyFrom", l_fairy_relations_copyFrom },
        { "IsEmpty", l_fairy_relations_isEmpty },
        { nullptr, nullptr }
    };
}
