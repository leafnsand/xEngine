#pragma once

#include "ui/Transition.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_transition_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.transition");
        return 1;
    }

    static int l_fairy_transition_getOwner(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getOwner());
            return 1;
        }
        return 0;
    }

    static int l_fairy_transition_isAutoPlay(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            lua_pushboolean(L, obj->isAutoPlay());
            return 1;
        }
        return 0;
    }

    static int l_fairy_transition_setAutoPlay(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            auto value = luaL_checkinteger(L, 2);
            obj->setAutoPlay(value);
        }
        return 0;
    }

    static int l_fairy_transition_isPlaying(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {

            lua_pushboolean(L, obj->isPlaying());
            return 1;
        }
        return 0;
    }

    static int l_fairy_transition_play(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            if (argc >= 3)
            {
                auto time = luaL_checkinteger(L, 2);
                auto delay = luaL_checknumber(L, 3);
                if (argc == 4)
                {
                    LuaRefCallBack callback(L, 4);
                    obj->play(time, delay,
                        [callback]() {
                        callback.call();
                    });
                }
                else
                {
                    obj->play(time, delay);
                }
            }
            else
            {
                if (argc == 2)
                {
                    LuaRefCallBack callback(L, 2);
                    obj->play([callback]() {
                        callback.call();
                    });
                }
                else
                {
                    obj->play();
                }
            }
        }
        return 0;
    }

    static int l_fairy_transition_playReverse(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            if (argc >= 3)
            {
                auto time = luaL_checkinteger(L, 2);
                auto delay = luaL_checknumber(L, 3);
                if (argc == 4)
                {
                    LuaRefCallBack callback(L, 4);
                    obj->playReverse(time, delay,
                        [callback]() {
                        callback.call();
                    });
                }
                else
                {
                    obj->playReverse(time, delay);
                }
            }
            else
            {
                if (argc == 1)
                {
                    LuaRefCallBack callback(L, 2);
                    obj->playReverse([callback]() {
                        callback.call();
                    });
                }
                else
                {
                    obj->playReverse();
                }
            }
        }
        return 0;
    }


    static int l_fairy_transition_changeRepeat(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            auto value = luaL_checkinteger(L, 2);
            obj->changeRepeat(value);
        }
        return 0;
    }

    static int l_fairy_transition_stop(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            if (argc == 3)
            {
                bool setToComplete = luaL_checkinteger(L, 2);
                bool processCallback = luaL_checkinteger(L, 3);
                obj->stop(setToComplete, processCallback);
            }
            else
            {
                obj->stop();
            }
        }
        return 0;
    }

    static int l_fairy_transition_setValue(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            size_t l0;
            const char* str = luaL_checklstring(L, 2, &l0);

            ValueVector vv(argc - 2);
            if (argc >= 3)
            {
                if (lua_isnumber(L,3))
                {
                    float originValue = luaL_checknumber(L, 3);
                    if (originValue == (int)originValue)
                    {
                        vv[0] = (int)originValue;
                    }
                    else 
                    {
                        vv[0] = originValue;
                    }
                }
                else
                {
                    size_t l;
                    const char* value0 = luaL_checklstring(L, 3, &l);
                    vv[0] = value0;
                }
            }
            if (argc >=4)
            {
                if (lua_isnumber(L, 4))
                {
                    float originValue = luaL_checknumber(L, 4);
                    if (originValue == (int)originValue)
                    {
                        vv[0] = (int)originValue;
                    }
                    else 
                    {
                        vv[0] = originValue;
                    }
                }
            }
            if (argc >3)
            {
                float value2 = luaL_checknumber(L, 5);
                float value3 = luaL_checknumber(L, 6);
                vv[2] = value2;
                vv[3] = value3;
            }
            obj->setValue(str, vv);
        }
        return 0;
    }

    static int l_fairy_transition_setHook(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            size_t l;
            const char* str = luaL_checklstring(L, 2, &l);
            LuaRefCallBack callback(L, 3);
            obj->setHook(str,
                [callback]() {
                callback.call();
            });
        }
        return 0;
    }

    static int l_fairy_transition_clearHooks(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            obj->clearHooks();
        }
        return 0;
    }

    static int l_fairy_transition_setTarget(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            size_t l;
            const char* str = luaL_checklstring(L, 2, &l);
            auto newTarget = n3lua_checkfairyclass<GObject>(L, 3, "fairy.object");
            obj->setTarget(str, newTarget);
        }
        return 0;
    }

    static int l_fairy_transition_setDuration(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            size_t l;
            const char* str = luaL_checklstring(L, 2, &l);
            auto value = luaL_checknumber(L, 3);
            obj->setDuration(str, value);
        }
        return 0;
    }


    static int l_fairy_transition_updateFromRelations(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            size_t l;
            const char* str = luaL_checklstring(L, 2, &l);
            auto dx = luaL_checknumber(L, 3);
            auto dy = luaL_checknumber(L, 4);
            obj->updateFromRelations(str, dx, dy);
        }
        return 0;
    }

    static int l_fairy_transition_OnOwnerRemovedFromStage(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Transition>(L, 1, "fairy.transition");
        if (obj)
        {
            obj->OnOwnerRemovedFromStage();
        }
        return 0;
    }

    static const luaL_Reg transition_method[] =
    {
        { "__tostring", l_fairy_transition_tostring },
        { "GetOwner", l_fairy_transition_getOwner },
        { "IsAutoPlay", l_fairy_transition_isAutoPlay },
        { "SetAutoPlay", l_fairy_transition_setAutoPlay },
        { "IsPlaying", l_fairy_transition_isPlaying },
        { "Play", l_fairy_transition_play },
        { "PlayReverse", l_fairy_transition_playReverse },
        { "ChangeRepeat", l_fairy_transition_changeRepeat },
        { "Stop", l_fairy_transition_stop },
        { "SetValue", l_fairy_transition_setValue },
        { "SetHook", l_fairy_transition_setHook },
        { "ClearHooks", l_fairy_transition_clearHooks },
        { "SetTarget", l_fairy_transition_setTarget },
        { "SetDuration", l_fairy_transition_setDuration },
        { "UpdateFromRelations", l_fairy_transition_updateFromRelations },
        { "OnOwnerRemovedFromStage", l_fairy_transition_OnOwnerRemovedFromStage },
        { nullptr, nullptr }
    };
}


