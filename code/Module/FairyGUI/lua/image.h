#pragma once

#include "ui/GImage.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_image_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.image");
        return 1;
    }

    static int l_fairy_image_getFlip(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GImage>(L, 1, "fairy.image");       
        if (obj)
        {
            lua_pushinteger(L, (int)obj->getFlip());
            return 1;
        }
        return 0;
    }

    static int l_fairy_image_setFlip(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GImage>(L, 1, "fairy.image");
        int value = luaL_checkinteger(L, 2);       
        if (obj)
        {
            obj->setFlip((FlipType)value);
        }
        return 0;
    }

    static int l_fairy_image_getColor(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GImage>(L, 1, "fairy.image");       
        if (obj)
        {
            n3lua_pushfairycolor(L, obj->getColor());
            return 1;
        }
        return 0;
    }

    static int l_fairy_image_setColor(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GImage>(L, 1, "fairy.image");
        Color4B color;
        n3lua_checkfairycolor(L, 2, color);       
        if (obj)
        {
            obj->setColor(color);
        }
        return 0;
    }

    static int l_fairy_image_constructFromResource(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GImage>(L, 1, "fairy.image");       
        if (obj)
        {
            obj->constructFromResource();
        }
        return 0;
    }
    
    static int l_fairy_image_getScale9GridRect(lua_State*L)
    {
        const auto obj = n3lua_checkfairyclass<GImage>(L, 1, "fairy.image");       
        if (obj)
        {
            n3lua_pushfairyrect(L, obj->getScale9GridRect());
            return 1;
        }
        return 0;
    }
    
    static const luaL_Reg image_method[] =
    {
        { "__tostring", l_fairy_image_tostring },
        { "GetFlip", l_fairy_image_getFlip },    
        { "SetFlip", l_fairy_image_setFlip },
        { "GetColor", l_fairy_image_getColor },
        { "SetColor", l_fairy_image_setColor },
        { "ConstructFromResource", l_fairy_image_constructFromResource },
        { "GetScale9GridRect", l_fairy_image_getScale9GridRect },
        { nullptr, nullptr }
    };
}

