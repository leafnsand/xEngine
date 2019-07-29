#pragma once
//------------------------------------------------------------------------------
/**
    @file n3lua/n3luautil.h
    @brief various helper inline methods
*/
//#include "core/types.h"

//#include "n3gfx/types.h"
//#include "math/float2.h"
//#include "math/float4.h"
//#include "math/transform44.h"

#include "AzCore/Math/Transform.h"

#include <AzCore/std/string/string.h>

#include "AzCore/Script/lua/lua.h"
#include "AzCore/Math/Vector2.h"
#include "AzCore/Math/Color.h"
#include "AzCore/Script/lua/lua.h"

#define LUA_OK		0

//--- set integer value from optional table field
inline void n3lua_int(lua_State* L, int n, const char* field, int& dst) {
    lua_getfield(L, n, field);
    if (lua_isnumber(L, -1))
    {
        dst = (float)luaL_checknumber(L, -1);
        if (dst % 1 == 0)
        {
            dst = (int)luaL_checkinteger(L, -1);
        }
    }
    lua_pop(L, 1);
}

//--- set integer value from optional table field
inline void n3lua_uint(lua_State* L, int n, const char* field, unsigned int& dst) {
    lua_getfield(L, n, field);
    if(lua_isnumber(L,-1))
    {
        dst = (float)luaL_checknumber(L, -1);
        if (dst % 1 == 0)
        {
            dst = (unsigned int)luaL_checkinteger(L, -1);
        }
    }
    lua_pop(L, 1);
}

//--- set float value from optional table field
inline void n3lua_float(lua_State* L, int n, const char* field, float& dst) {
    lua_getfield(L, n, field);
    if (lua_isnumber(L, -1)) {
        dst = (float) luaL_checknumber(L, -1);
    }
    lua_pop(L, 1);
}

//--- set bool value from optional table field
inline void n3lua_bool(lua_State* L, int n, const char* field, bool& dst) {
    lua_getfield(L, n, field);
    if (lua_isboolean(L, -1)) {
        dst = lua_toboolean(L, -1) != 0;
    }
    lua_pop(L, 1);
}

//--- set string from optional table field
inline void n3lua_string(lua_State* L, int n, const char* field, AZStd::string& dst) {
    lua_getfield(L, n, field);
    if (lua_isstring(L, -1)) {
        dst = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
}

//--- set stringatom from optional table field
inline void n3lua_stringatom(lua_State* L, int n, const char* field, AZStd::string& dst) {
    lua_getfield(L, n, field);
    if (lua_isstring(L, -1)) {
        dst = lua_tostring(L, -1);
    }
    lua_pop(L, 1);
}

//--- if table has field which is a function, write func to registry, and its unique key to provied ref
inline void n3lua_func(lua_State* L, int n, const char* field, int& dst) {
    lua_getfield(L, n, field);
    if (lua_isfunction(L, -1)) {
        dst = luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        lua_pop(L, 1);
    }
}

//--- set integer table field, table must be at top of stack
inline void n3lua_setint(lua_State* L, const char* field, int val) {
    lua_pushstring(L, field);
    lua_pushinteger(L, val);
    lua_settable(L, -3);        // this will also pop field and val
}

//--- set unsigned integer table field, table must be at top of stack
inline void n3lua_setuint(lua_State* L, const char* field, unsigned int val) {
    lua_pushstring(L, field);
    lua_pushinteger(L, (lua_Integer) val);
    lua_settable(L, 3);
}

//--- set integer table field, table must be at top of stack
inline void n3lua_setfloat(lua_State* L, const char* field, float val) {
    lua_pushstring(L, field);
    lua_pushnumber(L, val);
    lua_settable(L, -3);
}

//--- set boolean table field, table must be at top of stack
inline void n3lua_setbool(lua_State* L, const char* field, bool val) {
    lua_pushstring(L, field);
    lua_pushboolean(L, val);
    lua_settable(L, -3);
}

//--- set string table field, table must be at top of stack
inline void n3lua_setstr(lua_State* L, const char* field, const char* val) {
    lua_pushstring(L, field);
    lua_pushstring(L, val);
    lua_settable(L, -3);
}

//--- push integer if field name matches
inline bool n3lua_pushif_int(lua_State* L, const char* field, const char* match, int val) {
    AZStd::string ss(field);
    if (0 == ss.compare(match)) {
        lua_pushinteger(L, val);
        return true;
    }
    return false;
}

//--- push unsigned integer if field name matches
inline bool n3lua_pushif_uint(lua_State* L, const char* field, const char* match, unsigned int val) {
    AZStd::string ss(field);
    if (0 == ss.compare(match)) {
        lua_pushinteger(L, (lua_Integer) val);
        return true;
    }
    return false;
}

//--- push float if field name matches
inline bool n3lua_pushif_float(lua_State* L, const char* field, const char* match, float val) {
    AZStd::string ss(field);    
    if (0 == ss.compare(match)) {
        lua_pushnumber(L, val);
        return true;
    }
    return false;
}

//--- push boolean if field name matches
inline bool n3lua_pushif_bool(lua_State* L, const char* field, const char* match, bool val) {
    AZStd::string ss(field);
    if (0 == ss.compare(match)) {
        lua_pushboolean(L, (int) val);
        return true;
    }
    return false;
}

//--- push string if field name matches
inline bool n3lua_pushif_string(lua_State* L, const char* field, const char* match, const char* val) {
    AZStd::string ss(field);
    if (0 == ss.compare(match)) {
        lua_pushstring(L, val);
        return true;
    }
    return false;
}

#define n3lua_checkargc(L, n) { if (lua_gettop(L) > (n)) return luaL_error(L, "too many arguments!"); }

class LuaRefCallBack
{
public:
    LuaRefCallBack(lua_State* L, int index) : _l(L)
    {
        AZ_Assert(L,"");
        luaL_checktype(L, index, LUA_TFUNCTION);
        lua_pushvalue(L, index);
        _callback = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    ~LuaRefCallBack()
    {
        luaL_unref(_l, LUA_REGISTRYINDEX, _callback);
    }

    LuaRefCallBack(const LuaRefCallBack& rhv) : _l(rhv._l)
    {
        lua_rawgeti(_l, LUA_REGISTRYINDEX, rhv._callback);
        _callback = luaL_ref(_l, LUA_REGISTRYINDEX);
    }

    LuaRefCallBack(LuaRefCallBack&& rhv) noexcept :_l(rhv._l), _callback(rhv._callback)
    {
        rhv._callback = LUA_NOREF;
    }

    void call(int argc = 0) const
    {
        AZ_Assert(_l, "");
        lua_rawgeti(_l, LUA_REGISTRYINDEX, _callback);
        if (argc > 0)
        {
            lua_insert(_l, lua_gettop(_l) - argc);
        }
        if (LUA_OK != lua_pcall(_l, argc, 0, 0))
        {
            AZ_Warning("Lua","callback error:\n%s\n", luaL_checkstring(_l, -1));
        }
    }

private:
    lua_State* _l;
    int _callback;
};

inline void n3lua_addclass(lua_State* L, const char* name, const luaL_Reg* method, const char* base = nullptr)
{
    luaL_newmetatable(L, name);
    lua_getglobal(L, "_global_index");
    lua_setfield(L, -2, "__index");
    lua_getglobal(L, "_global_newindex");
    lua_setfield(L, -2, "__newindex");
    lua_getglobal(L, "_global_override_index");
    lua_setfield(L, -2, "_override_index");
    lua_getglobal(L, "_global_override_newindex");
    lua_setfield(L, -2, "_override_newindex");
    luaL_setfuncs(L, method, 0);
    if (base)
    {
        luaL_getmetatable(L, base);
        lua_setmetatable(L, -2);
    }
    lua_pop(L, 1);
}

template <typename T>
inline const T& n3lua_checkclass(lua_State* L, int index, const char* name)
{
    T* result = nullptr;
    int top = lua_gettop(L);
    void* ptr = lua_touserdata(L, index);
    if (!ptr)
    {
        luaL_argerror(L, index, "class not match");
    }
    luaL_getmetatable(L, name);
    int dst = lua_gettop(L);
    int n = index;
    while (lua_getmetatable(L, n))
    {
        if (lua_rawequal(L, -1, dst))
        {
            result = (T*) ptr;
            break;
        }
        n = lua_gettop(L);
    }
    lua_settop(L, top);
    if (result == nullptr)
    {
        luaL_argerror(L, index, "class not match");
    }
    return *result;
}

inline void* n3lua_checkptrclass(lua_State* L, int index, const char* name)
{
    void* result = nullptr;
    int top = lua_gettop(L);
    void** ptr = (void**)lua_touserdata(L, index);
    if (!ptr)
    {
        luaL_argerror(L, index, "class not match");
    }
    luaL_getmetatable(L, name);
    int dst = lua_gettop(L);
    int n = index;
    while (lua_getmetatable(L, n))
    {
        if (lua_rawequal(L, -1, dst))
        {
            result = (void*)(*ptr);
            break;
        }
        n = lua_gettop(L);
    }
    lua_settop(L, top);
    if (result == nullptr)
    {
        luaL_argerror(L, index, "class not match");
    }
    return result;
}

template <typename T>
inline void n3lua_pushclass(lua_State* L, const char* name, const T& instance, int(*gc)(lua_State*) = nullptr)
{
    T* pid = (T*)lua_newuserdata(L, sizeof(T));
    *pid = instance;
    luaL_newmetatable(L, name);
    if (gc)
    {
        lua_pushcfunction(L, gc);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);
}

inline void n3lua_pushptrclass(lua_State* L, const char* name, void* instance, int(*gc)(lua_State*) = nullptr)
{
    void** pid = (void**)lua_newuserdata(L, sizeof(void*));
    *pid = instance;
    luaL_newmetatable(L, name);
    if (gc)
    {
        lua_pushcfunction(L, gc);
        lua_setfield(L, -2, "__gc");
    }
    lua_setmetatable(L, -2);
}

inline void n3lua_totransform(lua_State* L, int index, AZ::Transform& tm)
{
    float x = 0, y = 0, sx = 1, sy = 1, r = 0;
    n3lua_float(L, index, "x", x);
    n3lua_float(L, index, "y", y);
    n3lua_float(L, index, "scaleX", sx);
    n3lua_float(L, index, "scaleY", sy);
    n3lua_float(L, index, "rotation", r);

    // tm.SetPosition(x, y, 0);
    // tm.
    // tm.setscale2d(Math::float2(sx, sy));
    // tm.setrotate2d(r * 3.14159265358979323846f / 180.0f);
}

inline void n3lua_checktransform(lua_State* L, int index, AZ::Transform& tm)
{
    if (lua_istable(L, index))
    {
        n3lua_totransform(L, index, tm);
    }
    else
    {
        luaL_argerror(L, index, "should be table like {x=0,y=0,scaleX=0,scaleY=0,rotation=0}");
    }
}

inline void n3lua_transform(lua_State* L, int index, const char* field, AZ::Transform& tm)
{
    lua_getfield(L, index, field);
    if (lua_istable(L, -1))
    {
        n3lua_totransform(L, -1, tm);
    }
    lua_pop(L, 1);
}

inline void n3lua_pushtransfrom(lua_State* L, const AZ::Transform& tm)
{
    lua_newtable(L);
    // fix me
    // n3lua_setfloat(L, "x", tm.getpositionx());
    // n3lua_setfloat(L, "y", tm.getpositiony());
    // n3lua_setfloat(L, "scaleX", tm.getscalex());
    // n3lua_setfloat(L, "scaleY", tm.getscaley());
    // n3lua_setfloat(L, "rotation", tm.getrotate2d());
}
/*
inline void n3lua_torect(lua_State* L, int index, GFX::Rect& rect)
{
    n3lua_float(L, index, "x", rect.x);
    n3lua_float(L, index, "y", rect.y);
    n3lua_float(L, index, "w", rect.w);
    n3lua_float(L, index, "h", rect.h);
}

inline void n3lua_checkrect(lua_State* L, int index, GFX::Rect& rect)
{
    if (lua_istable(L, index))
    {
        n3lua_torect(L, index, rect);
    }
    else
    {
        luaL_argerror(L, index, "should be table like {x=0,y=0,w=0,h=0}");
    }
}

inline void n3lua_rect(lua_State* L, int index, const char* field, GFX::Rect& rect)
{
    lua_getfield(L, index, field);
    if (lua_istable(L, -1))
    {
        n3lua_torect(L, -1, rect);
    }
    lua_pop(L, 1);
}

inline void n3lua_pushrect(lua_State* L, const GFX::Rect& rect)
{
    lua_newtable(L);
    n3lua_setfloat(L, "x", rect.x);
    n3lua_setfloat(L, "y", rect.y);
    n3lua_setfloat(L, "w", rect.w);
    n3lua_setfloat(L, "h", rect.h);
}

*/
inline void n3lua_tofloat2(lua_State* L, int index, AZ::Vector2& pos)
{
    float xy[2]{};
    n3lua_float(L, index, "x", xy[0]);
    n3lua_float(L, index, "y", xy[1]);
    pos.Set(xy[0], xy[1]);
}

inline void n3lua_checkfloat2(lua_State* L, int index, AZ::Vector2& pos)
{
    if (lua_istable(L, index))
    {
        n3lua_tofloat2(L, index, pos);
    }
    else
    {
        luaL_argerror(L, index, "should be table like {x=0,y=0}");
    }
}

inline void n3lua_float2(lua_State* L, int index, const char* field, AZ::Vector2& pos)
{
    lua_getfield(L, index, field);
    if (lua_istable(L, -1))
    {
        n3lua_tofloat2(L, -1, pos);
    }
    lua_pop(L, 1);
}

inline void n3lua_pushfloat2(lua_State* L, const AZ::Vector2& pos)
{
    lua_newtable(L);
    n3lua_setfloat(L, "x", pos.GetX());
    n3lua_setfloat(L, "y", pos.GetY());
}

inline void n3lua_setfloat2(lua_State* L, const char* field, const AZ::Vector2& pos) {
    lua_pushstring(L, field);
    n3lua_pushfloat2(L, pos);
    lua_settable(L, -3);
}

inline void n3lua_tocolor(lua_State* L, int index, AZ::Color& color)
{
    float rgba[4]{};
    n3lua_float(L, index, "r", rgba[0]);
    n3lua_float(L, index, "g", rgba[1]);
    n3lua_float(L, index, "b", rgba[2]);
    n3lua_float(L, index, "a", rgba[3]);
    color.Set(rgba[0], rgba[1], rgba[2], rgba[3]);
}

inline void n3lua_checkcolor(lua_State* L, int index, AZ::Color& color)
{
    if (lua_istable(L, index))
    {
        n3lua_tocolor(L, index, color);
    }
    else
    {
        luaL_argerror(L, index, "should be table like {r=0,g=0,b=0,a=0}");
    }
}

inline void n3lua_color(lua_State* L, int index, const char* field, AZ::Color& color)
{
    lua_getfield(L, index, field);
    if (lua_istable(L, -1))
    {
        n3lua_tocolor(L, -1, color);
    }
    lua_pop(L, 1);
}

inline void n3lua_pushcolor(lua_State* L, const AZ::Color& color)
{
    lua_newtable(L);
    n3lua_setfloat(L, "r", color.GetR());
    n3lua_setfloat(L, "g", color.GetG());
    n3lua_setfloat(L, "b", color.GetB());
    n3lua_setfloat(L, "a", color.GetA());
}
