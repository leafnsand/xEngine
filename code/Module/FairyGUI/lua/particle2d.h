#pragma once

#include "n3luautil.h"
#include "FairyGUI/extension/GParticle2d.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_particle2d_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.particle2d");
        return 1;
    }

    static int l_fairy_particle2d_getparticle2did(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GParticle2d>(L, 1, "fairy.particle2d");
        if (obj)
        {
            n3lua_pushclass(L, "particle2d", obj->GetParticleId());
            return 1;
        }
        */
        return 0;
    }

    static const luaL_Reg particle2d_method[] =
    {
        { "__tostring", l_fairy_particle2d_tostring },
        { "GetParticle2dObj", l_fairy_particle2d_getparticle2did },
        { nullptr, nullptr }
    };
}