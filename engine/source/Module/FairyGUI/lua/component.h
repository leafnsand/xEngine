#pragma once

#include "ui/GComponent.h"

#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_component_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.component");
        return 1;
    }

    static int l_fairy_component_AddChild(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (child && parent)
        {
            parent->addChild(child);
        }
        return 0;
    }

    static int l_fairy_component_AddChildAt(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto index = luaL_checkinteger(L, 3);
        if (child && parent)
        {
            parent->addChildAt(child, index);
        }
        return 0;
    }

    static int l_fairy_component_RemoveChild(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (child && parent)
        {
            parent->removeChild(child);
        }
        return 0;
    }

    static int l_fairy_component_RemoveChildAt(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto index = luaL_checkinteger(L, 2);
        if (parent)
        {
            parent->removeChildAt(index);
        }
        return 0;
    }

    static int l_fairy_component_RemoveChildren(lua_State* L)
    {

		int argc = lua_gettop(L);
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
		if (parent)
		{
			int num = parent->numChildren();
			const auto begin = ((argc >= 2) ? luaL_checkinteger(L, 2) : 0);
			const auto end = ((argc >= 3) ? luaL_checkinteger(L, 3) : num);
			parent->removeChildren(begin, end);
		}
        return 0;
    }

    static int l_fairy_component_GetChildAt(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto index = luaL_checkinteger(L, 2);
        if (parent)
        {
            GObject* obj = parent->getChildAt(index);
            if (obj)
            {
                n3lua_pushfairyclass(L, obj);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_GetChild(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto name = luaL_checkstring(L, 2);
        if (parent)
        {
            GObject* obj = parent->getChild(name);
            if (obj)
            {
                n3lua_pushfairyclass(L, obj);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_GetChildInGroup(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto group = n3lua_checkfairyclass<GGroup>(L, 2, "fairy.group");
        const auto name = luaL_checkstring(L, 3);
        if (group && parent)
        {
            GObject* obj = parent->getChildInGroup(group, name);
            if (obj)
            {
                n3lua_pushfairyclass(L, obj);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_GetChildById(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto id = luaL_checkstring(L, 2);
        if (parent)
        {
            GObject* obj = parent->getChildById(id);
            if (obj)
            {
                n3lua_pushfairyclass(L, obj);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_GetChildren(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            Vector<GObject*> arr = parent->getChildren();
            int size = (int)arr.size();
            lua_createtable(L, size, 0);
            if (size > 0)
            {
                for (int i = 0; i < size; ++i)
                {
                    n3lua_pushfairyclass(L, arr.at(i));
                    lua_rawseti(L, -2, i);
                }
            }
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetChildIndex(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (child && parent)
        {
            lua_pushinteger(L, parent->getChildIndex(child));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_SetChildIndexBefore(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto index = luaL_checkinteger(L, 3);
        if (child && parent)
        {
            parent->setChildIndexBefore(child, index);
        }
        return 0;
    }

    static int l_fairy_component_SwapChildren(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child1 = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto child2 = n3lua_checkfairyclass<GObject>(L, 3, "fairy.object");
        if (child1 && child2 && parent)
        {
            parent->swapChildren(child1, child2);
        }
        return 0;
    }

    static int l_fairy_component_SwapChildrenAt(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto index1 = luaL_checkinteger(L, 2);
        const auto index2 = luaL_checkinteger(L, 3);
        if (parent)
        {
            parent->swapChildrenAt(index1, index2);
        }
        return 0;
    }

    static int l_fairy_component_NumChildren(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushinteger(L, parent->numChildren());
			return 1;
        }
        return 0;
    }

    static int l_fairy_component_IsAncestorOf(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto obj = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (obj && parent)
        {
            lua_pushboolean(L, parent->isAncestorOf(obj));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_IsChildInView(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (child && parent)
        {
            lua_pushboolean(L, parent->isChildInView(child));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetFirstChildInView(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushinteger(L, parent->getFirstChildInView());
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_AddController(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");
        if (parent && controller)
        {
            parent->addController(controller);
        }
        return 0;
    }

    static int l_fairy_component_GetControllerAt(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checkinteger(L, 2);
        if (parent)
        {            
            n3lua_pushfairyclass(L, parent->getControllerAt(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetController(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checkstring(L, 2);
        if (parent)
        {
            n3lua_pushfairyclass(L, parent->getController(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetControllers(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            const auto result = parent->getControllers();
            lua_createtable(L, int(result.size()), 0);
            for (int i = 0; i < result.size(); ++i)
            {
                n3lua_pushfairyclass(L, result.at(i));
                lua_rawseti(L, -2, i + 1);
            }
            return 1;
        }        
        return 0;
    }

    static int l_fairy_component_RemoveController(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");
        if (parent && controller)
        {
            parent->removeController(controller);
        }
        return 0;
    }

    static int l_fairy_component_ApplyController(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");
        if (parent && controller)
        {
            parent->applyController(controller);
        }
        return 0;
    }

    static int l_fairy_component_ApplyAllControllers(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            parent->applyAllControllers();
        }
        return 0;
    }

    static int l_fairy_component_GetTransition(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checkstring(L, 2);
        if (parent)
        {
            n3lua_pushfairyclass(L,   parent->getTransition(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetTransitionAt(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checkinteger(L, 2);
        if (parent)
        {
            n3lua_pushfairyclass(L, parent->getTransitionAt(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetTransitions(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            const auto result = parent->getTransitions();
            lua_createtable(L, int(result.size()), 0);
            for (int i = 0; i < result.size(); ++i)
            {
                n3lua_pushfairyclass(L, result.at(i));
                lua_rawseti(L, -2, i + 1);
            }
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetOpaque(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushboolean(L, parent->getOpaque());
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_SetOpaque(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = lua_toboolean(L, 2) == 1;
        if (parent)
        {
            parent->setOpaque(value);
        }
        return 0;
    }

    static int l_fairy_component_GetMargin(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_component_SetMargin(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_component_GetChildrenRenderOrder(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushinteger(L, (int)parent->getChildrenRenderOrder());
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_SetChildrenRenderOrder(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checkinteger(L, 2);
        if (parent)
        {
            parent->setChildrenRenderOrder((ChildrenRenderOrder)value);
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_GetApexIndex(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushinteger(L, parent->getApexIndex());
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_SetApexIndex(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checkinteger(L, 2);
        if (parent)
        {
            parent->setApexIndex(value);
        }
        return 0;
    }

    static int l_fairy_component_GetMask(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            GObject* mask = parent->getMask();
            if (mask)
            {
                n3lua_pushfairyclass(L, mask);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_SetMask(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto mask = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (mask && parent)
        {
            parent->setMask(mask);
        }
        return 0;
    }

    static int l_fairy_component_GetHitArea(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_component_SetHitArea(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_component_GetScrollPane(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            ScrollPane* scrollpanel = parent->getScrollPane();
            if (scrollpanel)
            {
                n3lua_pushfairyclass(L, scrollpanel);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_GetViewWidth(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushnumber(L, parent->getViewWidth());
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_SetViewWidth(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checknumber(L, 2);
        if (parent)
        {
            parent->setViewWidth(value);
        }
        return 0;
    }

    static int l_fairy_component_GetViewHeight(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            lua_pushnumber(L, parent->getViewHeight());
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_SetViewHeight(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        const auto value = luaL_checknumber(L, 2);
        if (parent)
        {
            parent->setViewHeight(value);
        }
        return 0;
    }

    static int l_fairy_component_SetBoundsChangedFlag(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            parent->setBoundsChangedFlag();
        }
        return 0;
    }

    static int l_fairy_component_EnsureBoundsCorrect(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (parent)
        {
            parent->ensureBoundsCorrect();
        }
        return 0;
    }

    static int l_fairy_component_HitTest(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        Vec2 vec2;
        n3lua_checkfairyvec2(L, 2, vec2);
        if (parent)
        {
            GObject* obj = parent->hitTest(vec2);
            if (obj)
            {
                n3lua_pushfairyclass(L, obj);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_component_GetSnappingPosition(lua_State* L)
    {
        const auto parent = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        Vec2 vec2;
        n3lua_checkfairyvec2(L, 2, vec2);
        if (parent)
        {
            Vec2 value = parent->getSnappingPosition(vec2);
            n3lua_pushfairyvec2(L, value);
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_AddNewTransition(lua_State* L)
    {
        const auto component = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (component)
        {
            auto target = n3lua_checkfairyclass<Transition>(L, 2, "fairy.transition");
            auto object = n3lua_checkfairyclass<GObject>(L, 3, "fairy.object");
            Transition* t = component->addNewTransition(*target,*object);
            n3lua_pushfairyclass(L, t);
            return 1;
        }
        return 0;
    }

    static int l_fairy_component_RemoveTransition(lua_State* L) {
        const auto component = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (component)
        {
            const auto name = luaL_checkstring(L, 2);
            component->RemoveTransition(name);
        }
        return 0;
    }

    static const luaL_Reg component_method[] =
    {
        { "__tostring", l_fairy_component_tostring },
        { "AddChild", l_fairy_component_AddChild },
        { "AddChildAt", l_fairy_component_AddChildAt },
        { "RemoveChild", l_fairy_component_RemoveChild },
        { "RemoveChildAt", l_fairy_component_RemoveChildAt },
        { "RemoveChildren", l_fairy_component_RemoveChildren },
        { "GetChildAt", l_fairy_component_GetChildAt },
        { "GetChild", l_fairy_component_GetChild },
        { "GetChildInGroup", l_fairy_component_GetChildInGroup },
        { "GetChildById", l_fairy_component_GetChildById },
        { "GetChildren", l_fairy_component_GetChildren },
        { "GetChildIndex", l_fairy_component_GetChildIndex },
        { "SetChildIndexBefore", l_fairy_component_SetChildIndexBefore },
        { "SwapChildren", l_fairy_component_SwapChildren },
        { "SwapChildrenAt", l_fairy_component_SwapChildrenAt },
        { "NumChildren", l_fairy_component_NumChildren },
        { "IsAncestorOf", l_fairy_component_IsAncestorOf },
        { "IsChildInView", l_fairy_component_IsChildInView },
        { "GetFirstChildInView", l_fairy_component_GetFirstChildInView },
        { "AddController", l_fairy_component_AddController },
        { "GetControllerAt", l_fairy_component_GetControllerAt },
        { "GetController", l_fairy_component_GetController },
        { "GetControllers", l_fairy_component_GetControllers },
        { "RemoveController", l_fairy_component_RemoveController },
        { "ApplyController", l_fairy_component_ApplyController },
        { "ApplyAllControllers", l_fairy_component_ApplyAllControllers },
        { "GetTransition", l_fairy_component_GetTransition },
        { "GetTransitionAt", l_fairy_component_GetTransitionAt },
        { "GetTransitions", l_fairy_component_GetTransitions },
        { "GetOpaque", l_fairy_component_GetOpaque },
        { "SetOpaque", l_fairy_component_SetOpaque },
        { "GetMargin", l_fairy_component_GetMargin },
        { "SetMargin", l_fairy_component_SetMargin },
        { "GetChildrenRenderOrder", l_fairy_component_GetChildrenRenderOrder },
        { "SetChildrenRenderOrder", l_fairy_component_SetChildrenRenderOrder },
        { "GetApexIndex", l_fairy_component_GetApexIndex },
        { "SetApexIndex", l_fairy_component_SetApexIndex },
        { "GetMask", l_fairy_component_GetMask },
        { "SetMask", l_fairy_component_SetMask },
        { "GetHitArea", l_fairy_component_GetHitArea },
        { "SetHitArea", l_fairy_component_SetHitArea },
        { "GetScrollPane", l_fairy_component_GetScrollPane },
        { "GetViewWidth", l_fairy_component_GetViewWidth },
        { "SetViewWidth", l_fairy_component_SetViewWidth },
        { "GetViewHeight", l_fairy_component_GetViewHeight },
        { "SetViewHeight", l_fairy_component_SetViewHeight },
        { "SetBoundsChangedFlag", l_fairy_component_SetBoundsChangedFlag },
        { "EnsureBoundsCorrect", l_fairy_component_EnsureBoundsCorrect },
        { "HitTest", l_fairy_component_HitTest },
        { "GetSnappingPosition", l_fairy_component_GetSnappingPosition },
        { "AddNewTransition", l_fairy_component_AddNewTransition },
        { "RemoveTransition", l_fairy_component_RemoveTransition },
        { nullptr, nullptr }
    };
}