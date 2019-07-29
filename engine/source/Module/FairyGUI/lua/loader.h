#pragma once

#include "ui/GLoader.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_loader_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.loader");
        return 1;
    }

    static int l_fairy_loader_GetURL(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushstring(L, loader->getURL().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetURL(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkstring(L, 2);
            loader->setURL(value);
        }
        return 0;
    }

    static int l_fairy_loader_GetIcon(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushstring(L, loader->getIcon().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetIcon(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkstring(L, 2);
            loader->setIcon(value);
        }
        return 0;
    }

    static int l_fairy_loader_GetAlign(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushinteger(L, int(loader->getAlign()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetAlign(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setAlign(TextHAlignment(value));
        }
        return 0;
    }

    static int l_fairy_loader_GetVerticalAlign(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushinteger(L, int(loader->getVerticalAlign()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetVerticalAlign(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setVerticalAlign(TextVAlignment(value));
        }
        return 0;
    }

    static int l_fairy_loader_GetAutoSize(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushboolean(L, loader->getAutoSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetAutoSize(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setAutoSize(value);
        }
        return 0;
    }

    static int l_fairy_loader_GetFill(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushinteger(L, int(loader->getFill()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetFill(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setFill(LoaderFillType(value));
        }
        return 0;
    }

    static int l_fairy_loader_GetContentSize(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            n3lua_pushfairysize(L, loader->getContentSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_GetColor(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            n3lua_pushfairycolor(L, loader->getColor());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetColor(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            Color4B color;
            n3lua_checkfairycolor(L, 2, color);
            loader->setColor(color);
        }
        return 0;
    }

    static int l_fairy_loader_IsPlaying(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushboolean(L, loader->isPlaying());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetPlaying(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setPlaying(value);
        }
        return 0;
    }

    static int l_fairy_loader_GetCurrentFrame(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            lua_pushinteger(L, loader->getCurrentFrame());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_SetCurrentFrame(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setCurrentFrame(value);
        }
        return 0;
    }

    static int l_fairy_loader_SetAnchorPoint(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            Vec2 anchor;
            n3lua_checkfairyvec2(L, 2, anchor);
            loader->setAnchorPoint(anchor);
        }
        return 0;
    }

    static int l_fairy_loader_SetCascadeOpacityEnabled(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            const auto value = luaL_checkinteger(L, 2);
            loader->setCascadeOpacityEnabled(value);
        }
        return 0;
    }

    static int l_fairy_loader_SetSpriteFrame(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");
        const auto sprite = n3lua_checkfairyclass<SpriteFrame>(L, 2, "fairy.sprite");        
        if (loader && sprite)
        {
            loader->setSpriteFrame(sprite);
        }
        return 0;
    }

    static int l_fairy_loader_GetSpriteFrame(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
//            n3lua_pushfairyclass(L, loader->getSpriteFrame());
//            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_GetScale9GridRect(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            n3lua_pushfairyrect(L, loader->getScale9GridRect());
            return 1;
        }
        return 0;
    }

    static int l_fairy_loader_GetPackageItem(lua_State* L)
    {
        const auto loader = n3lua_checkfairyclass<GLoader>(L, 1, "fairy.loader");        
        if (loader)
        {
            n3lua_pushfairyclass(L, loader->getPackageItem());
            return 1;
        }
        return 0;
    }

    static const luaL_Reg loader_method[] =
    {
        { "__tostring", l_fairy_loader_tostring },
        { "GetURL", l_fairy_loader_GetURL },
        { "SetURL", l_fairy_loader_SetURL },
        { "GetIcon", l_fairy_loader_GetIcon },
        { "SetIcon", l_fairy_loader_SetIcon },
        { "GetAlign", l_fairy_loader_GetAlign },
        { "SetAlign", l_fairy_loader_SetAlign },
        { "GetVerticalAlign", l_fairy_loader_GetVerticalAlign },
        { "SetVerticalAlign", l_fairy_loader_SetVerticalAlign },
        { "GetAutoSize", l_fairy_loader_GetAutoSize },
        { "SetAutoSize", l_fairy_loader_SetAutoSize },
        { "GetFill", l_fairy_loader_GetFill },
        { "SetFill", l_fairy_loader_SetFill },
        { "GetContentSize", l_fairy_loader_GetContentSize },
        { "GetColor", l_fairy_loader_GetColor },
        { "SetColor", l_fairy_loader_SetColor },
        { "IsPlaying", l_fairy_loader_IsPlaying },
        { "SetPlaying", l_fairy_loader_SetPlaying },
        { "GetCurrentFrame", l_fairy_loader_GetCurrentFrame },
        { "SetCurrentFrame", l_fairy_loader_SetCurrentFrame },
        { "SetAnchorPoint", l_fairy_loader_SetAnchorPoint },
        { "SetCascadeOpacityEnabled", l_fairy_loader_SetCascadeOpacityEnabled },
        { "SetSpriteFrame", l_fairy_loader_SetSpriteFrame },
        { "GetSpriteFrame", l_fairy_loader_GetSpriteFrame },
        { "GetScale9GridRect", l_fairy_loader_GetScale9GridRect },
        { "GetPackageItem", l_fairy_loader_GetPackageItem },
        { nullptr, nullptr }
    };
}