#pragma once

#include "ui/GMovieClip.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_movieclip_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.movieclip");
        return 1;
    }

    static int l_fairy_movieclip_IsPlaying(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            lua_pushboolean(L, movieclip->isPlaying());
            return 1;
        }
        return 0;
    }

    static int l_fairy_movieclip_SetPlaying(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            const auto value = luaL_checkinteger(L, 2);
            movieclip->setPlaying(value);
        }
        return 0;
    }

    static int l_fairy_movieclip_GetCurrentFrame(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            lua_pushinteger(L, movieclip->getCurrentFrame());
            return 1;
        }
        return 0;
    }

    static int l_fairy_movieclip_SetCurrentFrame(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            const auto value = luaL_checkinteger(L, 2);
            movieclip->setCurrentFrame(value);
        }
        return 0;
    }

    static int l_fairy_movieclip_GetFlip(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            lua_pushinteger(L, int(movieclip->getFlip()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_movieclip_SetFlip(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            const auto value = luaL_checkinteger(L, 2);
            movieclip->setFlip(FlipType(value));
        }
        return 0;
    }

    static int l_fairy_movieclip_GetColor(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            n3lua_pushfairycolor(L, movieclip->getColor());
            return 1;
        }
        return 0;
    }

    static int l_fairy_movieclip_SetColor(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            Color4B color;
            n3lua_checkfairycolor(L, 2, color);
            movieclip->setColor(color);
        }
        return 0;
    }

    static int l_fairy_movieclip_SetPlaySettings(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            const auto start = luaL_checkinteger(L, 2);
            const auto end = luaL_checkinteger(L, 3);
            const auto times = luaL_checkinteger(L, 4);
            const auto endAt = luaL_checkinteger(L, 5);
            const auto callback = LuaRefCallBack(L, 6);
            movieclip->setPlaySettings(start, end, times, endAt, [callback]()
            {
                callback.call(0);
            });
        }
        return 0;
    }

    static int l_fairy_movieclip_ConstructFromResource(lua_State* L)
    {
        const auto movieclip = n3lua_checkfairyclass<GMovieClip>(L, 1, "fairy.movieclip");        
        if (movieclip)
        {
            movieclip->constructFromResource();
        }
        return 0;
    }

    static const luaL_Reg movieclip_method[] =
    {
        { "__tostring", l_fairy_movieclip_tostring },
        { "IsPlaying", l_fairy_movieclip_IsPlaying },
        { "SetPlaying", l_fairy_movieclip_SetPlaying },
        { "GetCurrentFrame", l_fairy_movieclip_GetCurrentFrame },
        { "SetCurrentFrame", l_fairy_movieclip_SetCurrentFrame },
        { "GetFlip", l_fairy_movieclip_GetFlip },
        { "SetFlip", l_fairy_movieclip_SetFlip },
        { "GetColor", l_fairy_movieclip_GetColor },
        { "SetColor", l_fairy_movieclip_SetColor },
        { "SetPlaySettings", l_fairy_movieclip_SetPlaySettings },
        { "ConstructFromResource", l_fairy_movieclip_ConstructFromResource },
        { nullptr, nullptr }
    };
}