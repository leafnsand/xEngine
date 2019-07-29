#pragma once

#include "event/InputEvent.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_inputevent_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.inputevent");
        return 1;
    }

    static int l_fairy_inputevent_getTarget(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getTarget());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushinteger(L, obj->getX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushinteger(L, obj->getY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getPosition(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getPosition());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getTouch(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
//            n3lua_pushfairyvec2(L, obj->getPosition());
//            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getTouchId(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushinteger(L, obj->getTouchId());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_isDoubleClick(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushboolean(L, obj->isDoubleClick());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getButton(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getButton());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getKeyCode(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getKeyCode());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_isCtrlDown(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushboolean(L, obj->isCtrlDown());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_isAltDown(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushboolean(L, obj->isAltDown());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_isShiftDown(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushboolean(L, obj->isShiftDown());
            return 1;
        }
        return 0;
    }

    static int l_fairy_inputevent_getMouseWheelDelta(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<InputEvent>(L, 1, "fairy.inputevent");
        if (obj)
        {
            lua_pushinteger(L, obj->getMouseWheelDelta());
            return 1;
        }
        return 0;
    }

    static const luaL_Reg inputevent_method[] =
    {
        { "__tostring", l_fairy_inputevent_tostring }, 
        { "GetTarget", l_fairy_inputevent_getTarget },
        { "GetX", l_fairy_inputevent_getX },
        { "GetY", l_fairy_inputevent_getY },
        { "GetPosition", l_fairy_inputevent_getPosition },
        { "GetTouch", l_fairy_inputevent_getTouch },
        { "GetTouchId", l_fairy_inputevent_getTouchId },
        { "IsDoubleClick", l_fairy_inputevent_isDoubleClick },
        { "GetButton", l_fairy_inputevent_getButton },
        { "GetKeyCode", l_fairy_inputevent_getKeyCode },
        { "IsCtrlDown", l_fairy_inputevent_isCtrlDown },
        { "IsAltDown", l_fairy_inputevent_isAltDown },
        { "IsShiftDown", l_fairy_inputevent_isShiftDown },
        { "GetMouseWheelDelta", l_fairy_inputevent_getMouseWheelDelta },
        { nullptr, nullptr }
    };
}

      