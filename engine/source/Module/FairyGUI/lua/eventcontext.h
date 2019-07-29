#pragma once


#include "event/UIEventDispatcher.h"
#include "util.h"


using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_eventcontext_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.eventcontext");
        return 1;
    }

    static int l_fairy_eventcontext_getType(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            lua_pushinteger(L, obj->getType());
            return 1;
        }
        return 0;
    }

    static int l_fairy_eventcontext_getSender(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getSender());
            return 1;
        }
        return 0;
    }

    static int l_fairy_eventcontext_getInput(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getInput());
            return 1;
        }
        return 0;
    }

    static int l_fairy_eventcontext_stopPropagation(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            obj->stopPropagation();
        }
        return 0;
    }

    static int l_fairy_eventcontext_preventDefault(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            obj->preventDefault();
        }
        return 0;
    }

    static int l_fairy_eventcontext_isDefaultPrevented(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            lua_pushboolean(L, obj->isDefaultPrevented());
            return 1;
        }
        return 0;
    }

    static int l_fairy_eventcontext_captureTouch(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            obj->captureTouch();
        }
        return 0;
    }

    static int l_fairy_eventcontext_uncaptureTouch(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            obj->uncaptureTouch();
        }
        return 0;
    }

    static int l_fairy_eventcontext_getDataValue(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
//            lua_pushboolean(L, obj->getDataValue());
//            return 1;
        }
        return 0;
    }

    static int l_fairy_eventcontext_getData(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<EventContext>(L, 1, "fairy.eventcontext");
        if (obj)
        {
            n3lua_pushfairyclass(L, (fairygui::Ref*)obj->getData());
            return 1;
        }
        return 0;
    }

    static const luaL_Reg eventcontext_method[] =
    {
        { "__tostring", l_fairy_eventcontext_tostring },
        { "GetType", l_fairy_eventcontext_getType },
        { "GetSender", l_fairy_eventcontext_getSender },
        { "GetInput", l_fairy_eventcontext_getInput },
        { "StopPropagation", l_fairy_eventcontext_stopPropagation },
        { "PreventDefault", l_fairy_eventcontext_preventDefault },
        { "IsDefaultPrevented", l_fairy_eventcontext_isDefaultPrevented },
        { "CaptureTouch", l_fairy_eventcontext_captureTouch },
        { "UncaptureTouch", l_fairy_eventcontext_uncaptureTouch },
        { "GetDataValue", l_fairy_eventcontext_getDataValue },
        { "GetData", l_fairy_eventcontext_getData },
        { nullptr, nullptr }
    };
}

        