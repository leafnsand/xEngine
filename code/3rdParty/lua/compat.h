#ifndef COMPAT_H
#define COMPAT_H

#include "lua.h"
#include "lauxlib.h"

#define luaL_newlibtable(L,l)	\
  lua_createtable(L, 0, sizeof(l)/sizeof((l)[0]) - 1)

#define luaL_newlib(L,l)  \
  (luaL_newlibtable(L,l), luaL_setfuncs(L,l,0))

LUA_API int   (lua_absindex) (lua_State *L, int idx);

LUALIB_API void (luaL_setfuncs) (lua_State *L, const luaL_Reg *l, int nup);

LUALIB_API int (luaL_getsubtable) (lua_State *L, int idx, const char *fname);

LUALIB_API void (luaL_requiref) (lua_State *L, const char *modname,
                                 lua_CFunction openf, int glb);

#endif
