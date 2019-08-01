#pragma once

#include "event/UIEventDispatcher.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_uieventdispatcher_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.uieventdispatcher");
        return 1;
    }

    static int l_fairy_uieventdispatcher_addEventListener(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        const auto value = luaL_checkinteger(L, 2);
        LuaRefCallBack callback(L, 3);
        if (obj)
        {
            if(argc == 3)
            {
                obj->addEventListener(value, [L, callback](EventContext* context)
                {
                    n3lua_pushfairyclass(L, context);
                    callback.call(1);
                });
            }
            else if(argc == 4)
            {
                const auto tag = luaL_checkinteger(L, 4);
                obj->addEventListener(value, [L, callback](EventContext* context)
                {
                    n3lua_pushfairyclass(L, context);
                    callback.call(1);
                }, (EventTag)tag);
            }
        }
        return 0;
    }

    static int l_fairy_uieventdispatcher_removeEventListener(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            if(argc == 2)
            {
                obj->removeEventListener(value);
            }
            else if(argc == 3)
            {
                const auto tag = luaL_checkinteger(L, 3);
                obj->removeEventListener(value, (EventTag)tag);
            }
        }
        return 0;
    }

    static int l_fairy_uieventdispatcher_removeEventListeners(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        if (obj)
        {
            obj->removeEventListeners();
        }
        return 0;
    }

    static int l_fairy_uieventdispatcher_hasEventListener(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            if (argc == 2)
            {
                lua_pushboolean(L , obj->hasEventListener(value));
                return 1;
            }
            else if (argc == 3)
            {
                const auto tag = luaL_checkinteger(L, 3);                ;
                lua_pushboolean(L, obj->hasEventListener(value, (EventTag)tag));
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_uieventdispatcher_dispatchEvent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            lua_pushboolean(L, obj->dispatchEvent(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_uieventdispatcher_bubbleEvent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            lua_pushboolean(L, obj->bubbleEvent(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_uieventdispatcher_isDispatchingEvent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<UIEventDispatcher>(L, 1, "fairy.uieventdispatcher");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            lua_pushboolean(L, obj->isDispatchingEvent(value));
            return 1;
        }
        return 0;
    }

    static const luaL_Reg uieventdispatcher_method[] =
    {
        { "__tostring", l_fairy_uieventdispatcher_tostring },
        { "AddEventListener", l_fairy_uieventdispatcher_addEventListener },
        { "RemoveEventListener", l_fairy_uieventdispatcher_removeEventListener },
        { "RemoveEventListeners", l_fairy_uieventdispatcher_removeEventListeners },
        { "HasEventListener", l_fairy_uieventdispatcher_hasEventListener },
        { "DispatchEvent", l_fairy_uieventdispatcher_dispatchEvent },
        { "BubbleEvent", l_fairy_uieventdispatcher_bubbleEvent },
        { "IsDispatchingEvent", l_fairy_uieventdispatcher_isDispatchingEvent },
        { nullptr, nullptr }
    };
}

        