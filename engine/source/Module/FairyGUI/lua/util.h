#pragma once

//#include "n3lua_fairy.h"
#include "n3luautil.h"
#include "FairyGUI/fairy/FairyGUI.h"

#include "FairyGUI/fairy/ui/GMovieClip.h"
#include "FairyGUI/fairy/ui/GImage.h"
#include "FairyGUI/fairy/ui/GTextField.h"
#include "FairyGUI/fairy/ui/Window.h"
#include "FairyGUI/fairy/ui/GLoader.h"
#include "FairyGUI/extension/GSpine.h"
#include "FairyGUI/extension/GParticle2d.h"
#include "FairyGUI/fairy/ui/GObjectPool.h"
#include "FairyGUI/fairy/ui/PopupMenu.h"
#include "FairyGUI/fairy/ui/GGraph.h"

#if __WIN32__
#pragma warning(disable: 4244)
#endif

using namespace fairygui;

namespace N3Lua
{
    template <typename T>
    inline T* n3lua_checkfairyclass(lua_State* L, int index, const char* name)
    {
        T* result = nullptr;
        int top = lua_gettop(L);
        if (!lua_isuserdata(L, index))
        {
            luaL_argerror(L, index, "not a fairy class");
            return result;
        }
        Ref** ptr = (Ref**)lua_touserdata(L, index);
        luaL_getmetatable(L, name);
        int dst = lua_gettop(L);
        int n = index;
        while (lua_getmetatable(L, n))
        {
            if (lua_rawequal(L, -1, dst) && (*ptr)->IsA<T>())
            {
                result = (*ptr)->SafeCast<T>();
                break;
            }
            n = lua_gettop(L);
        }
        lua_settop(L, top);
        if (result == nullptr)
        {
            luaL_argerror(L, index, "fairy class not match");
        }
        return result;
    }

    static int l_fairy_gc(lua_State* L)
    {
        Ref* ref = n3lua_checkfairyclass<Ref>(L, 1, "fairy.ref");
        ref->release();
        return 0;
    }

    inline void n3lua_pushfairyclass(lua_State*L, Ref* ptr)
    {
        if (ptr == nullptr)
        {
            lua_pushnil(L);
            return;
        }

        ptr->retain();

        const auto ref = (Ref**)lua_newuserdata(L, sizeof(Ref*));
        *ref = ptr;

        if (ptr->IsA<GButton>())
        {
            luaL_newmetatable(L, "fairy.button");
        }
        else if (ptr->IsA<GComboBox>())
        {
            luaL_newmetatable(L, "fairy.combobox");
        }
        else if (ptr->IsA<GLabel>())
        {
            luaL_newmetatable(L, "fairy.label");
        }
        else if (ptr->IsA<GList>())
        {
            luaL_newmetatable(L, "fairy.list");
        }
        else if (ptr->IsA<GProgressBar>())
        {
            luaL_newmetatable(L, "fairy.progressbar");
        }
        else if (ptr->IsA<GRoot>())
        {
            luaL_newmetatable(L, "fairy.root");
        }
        else if (ptr->IsA<GScrollBar>())
        {
            luaL_newmetatable(L, "fairy.scrollbar");
        }
        else if (ptr->IsA<GSlider>())
        {
            luaL_newmetatable(L, "fairy.slider");
        }
        else if (ptr->IsA<Window>())
        {
            luaL_newmetatable(L, "fairy.window");
        }
        else if (ptr->IsA<GComponent>())
        {
            luaL_newmetatable(L, "fairy.component");
        }
        else if (ptr->IsA<GGroup>())
        {
            luaL_newmetatable(L, "fairy.group");
        }
        else if (ptr->IsA<GImage>())
        {
            luaL_newmetatable(L, "fairy.image");
        }
        else if (ptr->IsA<GLoader>())
        {
            luaL_newmetatable(L, "fairy.loader");
        }
        else if (ptr->IsA<GMovieClip>())
        {
            luaL_newmetatable(L, "fairy.movieclip");
        }
        else if (ptr->IsA<GBasicTextField>())
        {
            luaL_newmetatable(L, "fairy.basictextfield");
        }
        else if (ptr->IsA<GRichTextField>())
        {
            luaL_newmetatable(L, "fairy.richtextfield");
        }
        else if (ptr->IsA<GTextInput>())
        {
            luaL_newmetatable(L, "fairy.textinput");
        }
        else if (ptr->IsA<GTextField>())
        {
            luaL_newmetatable(L, "fairy.textfield");
        }
        else if (ptr->IsA<GSpine>())
        {
            luaL_newmetatable(L, "fairy.spine");
        }
        else if (ptr->IsA<GParticle2d>())
        {
            luaL_newmetatable(L, "fairy.particle2d");
        }
        else if (ptr->IsA<ScrollPane>())
        {
            luaL_newmetatable(L, "fairy.scrollpane");
        }
        else if (ptr->IsA<GController>())
        {
            luaL_newmetatable(L, "fairy.controller");
        }
        else if (ptr->IsA<EventContext>())
        {
            luaL_newmetatable(L, "fairy.eventcontext");
        }
        else if (ptr->IsA<InputEvent>())
        {
            luaL_newmetatable(L, "fairy.inputevent");
        }
        else if (ptr->IsA<GObjectPool>())
        {
            luaL_newmetatable(L, "fairy.objectpool");
        }
        else if (ptr->IsA<PopupMenu>())
        {
            luaL_newmetatable(L, "fairy.popupmenu");
        }
        else if (ptr->IsA<GGraph>())
        {
            luaL_newmetatable(L, "fairy.graph");
        }
        else if (ptr->IsA<GObject>())
        {
            luaL_newmetatable(L, "fairy.object");
        }
        else if (ptr->IsA<Relations>())
        {
            luaL_newmetatable(L, "fairy.relations");
        }
        else if (ptr->IsA<Transition>()) 
        {
            luaL_newmetatable(L, "fairy.transition");
        }
        else
        {
            luaL_newmetatable(L, "fairy.ref");
        }

        lua_pushcfunction(L, l_fairy_gc);
        lua_setfield(L, -2, "__gc");
        lua_setmetatable(L, -2);
    }

    inline void n3lua_tofairycolor(lua_State* L, int index, Color3B& color)
    {
        float r, g, b;
        n3lua_float(L, index, "r", r);
        n3lua_float(L, index, "g", g);
        n3lua_float(L, index, "b", b);
        color.r = GLubyte(r * 255);
        color.g = GLubyte(g * 255);
        color.b = GLubyte(b * 255);
    }

    inline void n3lua_checkfairycolor(lua_State* L, int index, Color3B& color)
    {
        if (lua_istable(L, index))
        {
            n3lua_tofairycolor(L, index, color);
        }
        else
        {
            luaL_argerror(L, index, "should be table like {r=0,g=0,b=0,a=0}");
        }
    }

    inline void n3lua_fairycolor(lua_State* L, int index, const char* field, Color3B& color)
    {
        lua_getfield(L, index, field);
        if (lua_istable(L, -1))
        {
            n3lua_tofairycolor(L, -1, color);
        }
        lua_pop(L, 1);
    }

    inline void n3lua_pushfairycolor(lua_State* L, const Color3B& color)
    {
        lua_newtable(L);
        n3lua_setfloat(L, "r", color.r / 255);
        n3lua_setfloat(L, "g", color.g / 255);
        n3lua_setfloat(L, "b", color.b / 255);
    }

    inline void n3lua_tofairycolor(lua_State* L, int index, Color4B& color)
    {
        float r, g, b, a;
        n3lua_float(L, index, "r", r);
        n3lua_float(L, index, "g", g);
        n3lua_float(L, index, "b", b);
        n3lua_float(L, index, "a", a);
        color.r = GLubyte(r * 255);
        color.g = GLubyte(g * 255);
        color.b = GLubyte(b * 255);
        color.a = GLubyte(a * 255);
    }

    inline void n3lua_checkfairycolor(lua_State* L, int index, Color4B& color)
    {
        if (lua_istable(L, index))
        {
            n3lua_tofairycolor(L, index, color);
        }
        else
        {
            luaL_argerror(L, index, "should be table like {r=0,g=0,b=0,a=0}");
        }
    }

    inline void n3lua_fairycolor(lua_State* L, int index, const char* field, Color4B& color)
    {
        lua_getfield(L, index, field);
        if (lua_istable(L, -1))
        {
            n3lua_tofairycolor(L, -1, color);
        }
        lua_pop(L, 1);
    }

    inline void n3lua_pushfairycolor(lua_State* L, const Color4B& color)
    {
        lua_newtable(L);
        n3lua_setfloat(L, "r", color.r / 255);
        n3lua_setfloat(L, "g", color.g / 255);
        n3lua_setfloat(L, "b", color.b / 255);
        n3lua_setfloat(L, "a", color.a / 255);
    }
    
    inline void n3lua_setfairycolor(lua_State* L, const char* field, const Color4B& color) {
        lua_pushstring(L, field);
        n3lua_pushfairycolor(L, color);
        lua_settable(L, -3);
    }

    inline void n3lua_tofairyvec2(lua_State* L, int index, Vec2& vec)
    {
        n3lua_float(L, index, "x", vec.x);
        n3lua_float(L, index, "y", vec.y);
    }

    inline void n3lua_checkfairyvec2(lua_State* L, int index, Vec2& vec)
    {
        if (lua_istable(L, index))
        {
            n3lua_tofairyvec2(L, index, vec);
        }
        else
        {
            luaL_argerror(L, index, "should be table like {x=0,y=0}");
        }
    }

    inline void n3lua_fairyvec2(lua_State* L, int index, const char* field, Vec2& vec)
    {
        lua_getfield(L, index, field);
        if (lua_istable(L, -1))
        {
            n3lua_tofairyvec2(L, -1, vec);
        }
        lua_pop(L, 1);
    }

    inline void n3lua_pushfairyvec2(lua_State* L, const Vec2& vec)
    {
        lua_newtable(L);
        n3lua_setfloat(L, "x", vec.x);
        n3lua_setfloat(L, "y", vec.y);
    }

    inline void n3lua_setfairyvec2(lua_State* L, const char* field, const Vec2& vec) {
        lua_pushstring(L, field);
        n3lua_pushfairyvec2(L, vec);
        lua_settable(L, -3);
    }

    inline void n3lua_tofairysize(lua_State* L, int index, Size& size)
    {
        n3lua_float(L, index, "w", size.width);
        n3lua_float(L, index, "h", size.height);
    }

    inline void n3lua_checkfairysize(lua_State* L, int index, Size& size)
    {
        if (lua_istable(L, index))
        {
            n3lua_tofairysize(L, index, size);
        }
        else
        {
            luaL_argerror(L, index, "should be table like {w=0,h=0}");
        }
    }

    inline void n3lua_fairysize(lua_State* L, int index, const char* field, Size& size)
    {
        lua_getfield(L, index, field);
        if (lua_istable(L, -1))
        {
            n3lua_tofairysize(L, -1, size);
        }
        lua_pop(L, 1);
    }

    inline void n3lua_pushfairysize(lua_State* L, const Size& size)
    {
        lua_newtable(L);
        n3lua_setfloat(L, "w", size.width);
        n3lua_setfloat(L, "h", size.height);
    }

    inline void n3lua_setfairysize(lua_State* L, const char* field, const Size& size) {
        lua_pushstring(L, field);
        n3lua_pushfairysize(L, size);
        lua_settable(L, -3);
    }

    inline void n3lua_tofairyrect(lua_State* L, int index, Rect& rect)
    {
        n3lua_fairysize(L, index, "size", rect.size);
        n3lua_fairyvec2(L, index, "origin", rect.origin);
    }

    inline void n3lua_checkfairyrect(lua_State* L, int index, Rect& rect)
    {
        if (lua_istable(L, index))
        {
            n3lua_tofairyrect(L, index, rect);
        }
        else
        {
            luaL_argerror(L, index, "should be table like {size={w=0,h=0},origin={x=0,y=0}}");
        }
    }

    inline void n3lua_fairyrect(lua_State* L, int index, const char* field, Rect& rect)
    {
        lua_getfield(L, index, field);
        if (lua_istable(L, -1))
        {
            n3lua_tofairyrect(L, -1, rect);
        }
        lua_pop(L, 1);
    }

    inline void n3lua_pushfairyrect(lua_State* L, const Rect& rect)
    {
        lua_newtable(L);
        n3lua_setfairysize(L, "size", rect.size);
        n3lua_setfairyvec2(L, "origin", rect.origin);
    }

    inline void n3lua_setfairyrect(lua_State* L, const char* field, const Rect& rect) {
        lua_pushstring(L, field);
        n3lua_pushfairyrect(L, rect);
        lua_settable(L, -3);
    }
} // namespace N3Lua
