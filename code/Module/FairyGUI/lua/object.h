#pragma once



#include "ui/GObject.h"
#include "ui/GGroup.h"
#include "util.h"


namespace N3Lua
{
    static int l_fairy_object_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.object");
        return 1;
    }

    static int l_fairy_object_GetX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if(obj)
        {
            lua_pushinteger(L, obj->getX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setX(value);
        }
        return 0;
    }

    static int l_fairy_object_GetY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushinteger(L, obj->getY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setY(value);
        }
        return 0;
    }

    static int l_fairy_object_GetPosition(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getPosition());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetPosition(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);
        if (obj)
        {
            obj->setPosition(valueX, valueY);
        }
        return 0;
    }

    static int l_fairy_object_GetPixelSnapping(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushboolean(L, obj->getPixelSnapping());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetPixelSnapping(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setPixelSnapping(value);
        }
        return 0;
    }

    static int l_fairy_object_GetWidth(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushinteger(L, obj->getWidth());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetWidth(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setWidth(value);
        }
        return 0;
    }

    static int l_fairy_object_GetHeight(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushinteger(L, obj->getHeight());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetHeight(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setHeight(value);
        }
        return 0;
    }

    static int l_fairy_object_GetSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairysize(L, obj->getSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetSize(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);
        const auto ignore = ((argc == 4) ? lua_toboolean(L, 4) : false);
        if (obj)
        {
            obj->setSize(valueX, valueY, ignore);
        }
        return 0;
    }

    static int l_fairy_object_Center(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_object_MakeFullScreen(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            obj->makeFullScreen();
        }
        return 0;
    }

    static int l_fairy_object_GetPivot(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getPivot());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetPivot(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);
        const auto asAnchor = ((argc == 4) ? lua_toboolean(L, 4) : true);
        if (obj)
        {
            obj->setPivot(valueX, valueY, asAnchor);
        }
        return 0;
    }

    static int l_fairy_object_GetScaleX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushnumber(L, obj->getScaleX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetScaleX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setScaleX(value);
        }
        return 0;
    }

    static int l_fairy_object_GetScaleY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushnumber(L, obj->getScaleY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetScaleY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setScaleY(value);
        }
        return 0;
    }

    static int l_fairy_object_GetScale(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getScale());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetScale(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);
        if (obj)
        {
            obj->setScale(valueX, valueY);
        }
        return 0;
    }

    static int l_fairy_object_GetSkewX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushnumber(L, obj->getSkewX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetSkewX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setSkewX(value);
        }
        return 0;
    }

    static int l_fairy_object_GetSkewY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushnumber(L, obj->getSkewY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetSkewY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setSkewY(value);
        }
        return 0;
    }

    static int l_fairy_object_GetSkew(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getSkew());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetSkew(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);
        if (obj)
        {
            obj->setSkew(valueX, valueY);
        }
        return 0;
    }

    static int l_fairy_object_GetRotation(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushnumber(L, obj->getRotation());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetRotation(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setRotation(value);
        }
        return 0;
    }

    static int l_fairy_object_GetAlpha(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushnumber(L, obj->getAlpha());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetAlpha(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setAlpha(value);
        }
        return 0;
    }

    static int l_fairy_object_IsGrayed(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushboolean(L, obj->isGrayed());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetGrayed(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setGrayed(value);
        }
        return 0;
    }

    static int l_fairy_object_IsVisible(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushboolean(L, obj->isVisible());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetVisible(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setVisible(value);
        }
        return 0;
    }

    static int l_fairy_object_IsTouchable(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushboolean(L, obj->isTouchable());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetTouchable(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setTouchable(value);
        }
        return 0;
    }

    static int l_fairy_object_GetSortingOrder(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushinteger(L, obj->getSortingOrder());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetSortingOrder(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setSortingOrder(value);
        }
        return 0;
    }

    static int l_fairy_object_GetGroup(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            GGroup* group = obj->getGroup();
            if (group)
            {
                n3lua_pushfairyclass(L, group);
                return 1;
            }
        }
        return 0;
    }

    static int l_fairy_object_SetGroup(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto group = n3lua_checkfairyclass<GGroup>(L, 2, "fairy.group");
        if (obj && group)
        {
            obj->setGroup(group);
        }
        return 0;
    }

    static int l_fairy_object_GetText(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_object_SetText(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_object_GetIcon(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_object_SetIcon(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_object_GetTooltips(lua_State* L)
    {
        return 0;
    }

    static int l_fairy_object_SetTooltips(lua_State* L)
    {
        
        return 0;
    }

    static int l_fairy_object_GetData(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if(obj)
        {
            lua_pushstring(L,obj->customData.c_str());
            return 1;
        }
        */
        return 0;
    }

    static int l_fairy_object_SetData(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            const char *data = luaL_checkstring(L, 2);
            obj->customData = data;
        }
        */
        return 0;
    }

    static int l_fairy_object_IsDraggable(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushboolean(L, obj->isDraggable());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetDraggable(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto value = lua_toboolean(L, 2) == 1;
        if (obj)
        {
            obj->setDraggable(value);
        }
        return 0;
    }

    static int l_fairy_object_GetDragBounds(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairyrect(L, *(obj->getDragBounds()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_SetDragBounds(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        Rect rect;
        n3lua_checkfairyrect(L, 2, rect);
        if (obj)
        {
            obj->setDragBounds(rect);
        }
        return 0;
    }

    static int l_fairy_object_StartDrag(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            obj->startDrag();
        }
        return 0;
    }

    static int l_fairy_object_StopDrag(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            obj->stopDrag();
        }
        return 0;
    }

    static int l_fairy_object_AddEventListener(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto eventType = luaL_checkinteger(L, 2);
        if(obj)
        {
            LuaRefCallBack callback(L, 3);
            obj->addEventListener(eventType, [L, callback](EventContext* context)
            {
                n3lua_pushfairyclass(L, context);
                callback.call(1);
            });
        }
        return 0;
    }

    static int l_fairy_object_RemoveEventListener(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto eventType = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->removeEventListener(eventType);
        }
        return 0;
    }

    static int l_fairy_object_RemoveEventListeners(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            obj->removeEventListeners();
        }
        return 0;
    }

    static int l_fairy_object_HasEventListener(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        const auto eventType = luaL_checkinteger(L, 2);
        if (obj)
        {
            lua_pushboolean(L, obj->hasEventListener(eventType));
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_GetParent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getParent());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_RemoveFromParent(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            obj->removeFromParent();
        }
        return 0;
    }

	static int l_fairy_object_Relations(lua_State* L)
	{
		const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
		if (obj)
		{
			n3lua_pushfairyclass(L, obj->relations());
			return 1;
		}
		return 0;
	}

	static int l_fairy_object_AddRelation(lua_State* L)
	{
		int argc = lua_gettop(L);
		const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
		const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
		const auto type = luaL_checkinteger(L, 3);
		const auto isPer = (argc == 4) ? luaL_checkinteger(L, 4) : false;
		if (obj && target)
		{
			obj->addRelation(target, (RelationType)type, isPer);
		}
		return 0;
	}

	static int l_fairy_object_RemoveRelation(lua_State* L)
	{
		const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
		const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
		const auto type = luaL_checkinteger(L, 3);
		if (obj && target)
		{
			obj->removeRelation(target, (RelationType)type);
		}
		return 0;
	}

    static int l_fairy_object_ConvertToWorldSpace(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            const auto valueX = luaL_checknumber(L, 2);
            const auto valueY = luaL_checknumber(L, 3);
            n3lua_pushfairyvec2(L, obj->convertToWorldSpace(Vec2(valueX, valueY)));
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_ConvertToNodeSpace(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            const auto valueX = luaL_checknumber(L, 2);
            const auto valueY = luaL_checknumber(L, 3);
            n3lua_pushfairyvec2(L, obj->convertToNodeSpace(Vec2(valueX, valueY)));
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_convertToWorldSpaceRotation(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            const float value = luaL_checknumber(L, 2);
            lua_pushnumber(L, obj->convertToWorldSpaceRotation(value));
            return 1;
        }
        */
        return 0;
    }

    static int l_fairy_object_convertToNodeSpaceRotation(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            const float value = luaL_checknumber(L, 2);
            lua_pushnumber(L, obj->convertToNodeSpaceRotation(value));
            return 1;
        }
        */
        return 0;
    }

    static int l_fairy_object_GetName(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            lua_pushstring(L, obj->name.c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_object_RefreshTransformImmediately(lua_State* L)
    {
        // todo...
        /*
        const auto obj = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj)
        {
            obj->RefreshTransformImmediately();
        }
        */
        return 0;
    }

    static const luaL_Reg object_method[] =
    {
        { "__tostring", l_fairy_object_tostring },
        { "GetName", l_fairy_object_GetName },
        { "GetX", l_fairy_object_GetX },
        { "SetX", l_fairy_object_SetX },
        { "GetY", l_fairy_object_GetY },
        { "SetY", l_fairy_object_SetY },
        { "GetPosition", l_fairy_object_GetPosition },
        { "SetPosition", l_fairy_object_SetPosition },
        { "GetPixelSnapping", l_fairy_object_GetPixelSnapping },
        { "SetPixelSnapping", l_fairy_object_SetPixelSnapping },
        { "GetWidth", l_fairy_object_GetWidth },
        { "SetWidth", l_fairy_object_SetWidth },
        { "GetHeight", l_fairy_object_GetHeight },
        { "SetHeight", l_fairy_object_SetHeight },
        { "GetSize", l_fairy_object_GetSize },
        { "SetSize", l_fairy_object_SetSize },
        { "Center", l_fairy_object_Center },
        { "MakeFullScreen", l_fairy_object_MakeFullScreen },
        { "GetPivot", l_fairy_object_GetPivot },
        { "SetPivot", l_fairy_object_SetPivot },
        { "GetScaleX", l_fairy_object_GetScaleX },
        { "SetScaleX", l_fairy_object_SetScaleX },
        { "GetScaleY", l_fairy_object_GetScaleY },
        { "SetScaleY", l_fairy_object_SetScaleY },
        { "GetScale", l_fairy_object_GetScale },
        { "SetScale", l_fairy_object_SetScale },
        { "GetSkewX", l_fairy_object_GetSkewX },
        { "SetSkewX", l_fairy_object_SetSkewX },
        { "GetSkewY", l_fairy_object_GetSkewY },
        { "SetSkewY", l_fairy_object_SetSkewY },
        { "GetSkew", l_fairy_object_GetSkew },
        { "SetSkew", l_fairy_object_SetSkew },
        { "GetRotation", l_fairy_object_GetRotation },
        { "SetRotation", l_fairy_object_SetRotation },
        { "GetAlpha", l_fairy_object_GetAlpha },
        { "SetAlpha", l_fairy_object_SetAlpha },
        { "IsGrayed", l_fairy_object_IsGrayed },
        { "SetGrayed", l_fairy_object_SetGrayed },
        { "IsVisible", l_fairy_object_IsVisible },
        { "SetVisible", l_fairy_object_SetVisible },
        { "IsTouchable", l_fairy_object_IsTouchable },
        { "SetTouchable", l_fairy_object_SetTouchable },
        { "GetSortingOrder", l_fairy_object_GetSortingOrder },
        { "SetSortingOrder", l_fairy_object_SetSortingOrder },
        { "GetGroup", l_fairy_object_GetGroup },
        { "SetGroup", l_fairy_object_SetGroup },
        { "GetText", l_fairy_object_GetText },
        { "SetText", l_fairy_object_SetText },
        { "GetIcon", l_fairy_object_GetIcon },
        { "SetIcon", l_fairy_object_SetIcon },
        { "GetTooltips", l_fairy_object_GetTooltips },
        { "SetTooltips", l_fairy_object_SetTooltips },
        { "GetData", l_fairy_object_GetData },
        { "SetData", l_fairy_object_SetData },
        { "IsDraggable", l_fairy_object_IsDraggable },
        { "SetDraggable", l_fairy_object_SetDraggable },
        { "GetDragBounds", l_fairy_object_GetDragBounds },
        { "SetDragBounds", l_fairy_object_SetDragBounds },
        { "StartDrag", l_fairy_object_StartDrag },
        { "StopDrag", l_fairy_object_StopDrag },
        { "AddEventListener", l_fairy_object_AddEventListener },
        { "RemoveEventListener", l_fairy_object_RemoveEventListener },
        { "RemoveEventListeners", l_fairy_object_RemoveEventListeners },
        { "HasEventListener", l_fairy_object_HasEventListener },
        { "GetParent", l_fairy_object_GetParent },
        { "RemoveFromParent", l_fairy_object_RemoveFromParent },
		{ "Relations", l_fairy_object_Relations },
		{ "AddRelation", l_fairy_object_AddRelation },
		{ "RemoveRelation", l_fairy_object_RemoveRelation },
        { "ConvertToWorldSpace", l_fairy_object_ConvertToWorldSpace },
        { "ConvertToNodeSpace", l_fairy_object_ConvertToNodeSpace },
        { "ConvertToWorldSpaceRotation", l_fairy_object_convertToWorldSpaceRotation },
        { "ConvertToNodeSpaceRotation", l_fairy_object_convertToNodeSpaceRotation },
        { "RefreshTransformImmediately", l_fairy_object_RefreshTransformImmediately },
        { nullptr, nullptr }
    };
}