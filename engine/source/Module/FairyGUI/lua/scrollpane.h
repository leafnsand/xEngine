#pragma once

#include "ui/ScrollPane.h"
#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_scrollpane_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.scrollpane");
        return 1;
    }
    
    static int l_fairy_scrollpane_getOwner(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getOwner());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getHeader(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getHeader());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getFooter(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getFooter());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getVtScrollBar(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getVtScrollBar());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getHzScrollBar(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getHzScrollBar());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_isBouncebackEffect(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isBouncebackEffect());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setBouncebackEffect(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setBouncebackEffect(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isTouchEffect(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isTouchEffect());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setTouchEffect(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setTouchEffect(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isInertiaDisabled(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isInertiaDisabled());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setInertiaDisabled(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setInertiaDisabled(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_getScrollStep(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getScrollStep());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setScrollStep(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setScrollStep(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isSnapToItem(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isSnapToItem());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setSnapToItem(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setSnapToItem(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isPageMode(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isPageMode());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPageMode(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setPageMode(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPageController(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getPageController());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPageController(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");
        if (obj && controller)
        {
            obj->setPageController(controller);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isMouseWheelEnabled(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isMouseWheelEnabled());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setMouseWheelEnabled(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            obj->setMouseWheelEnabled(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_getDecelerationRate(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getDecelerationRate());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setDecelerationRate(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setDecelerationRate(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPosX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getPosX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPosX(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            if(argc == 2)
            {
                obj->setPosX(value);
            }
            else if(argc == 3)
            {
                const auto b = luaL_checkinteger(L, 3);
                obj->setPosX(value, b);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPosY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getPosY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPosY(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            if (argc == 2)
            {
                obj->setPosY(value);
            }
            else if (argc == 3)
            {
                const auto b = luaL_checkinteger(L, 3);
                obj->setPosY(value, b);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPos(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getPos());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPos(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            if (argc == 2)
            {
                Vec2 vec;
                n3lua_checkfairyvec2(L, 2, vec);
                obj->setPos(vec);
            }
            else if (argc == 3)
            {
                const auto valueX = luaL_checknumber(L, 2);
                const auto valueY = luaL_checknumber(L, 3);
                obj->setPos(valueX, valueY);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPercX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getPercX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPercX(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            if (argc == 2)
            {
                obj->setPercX(value);
            }
            else if (argc == 3)
            {
                const auto b = luaL_checkinteger(L, 3);
                obj->setPercX(value, b);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPercY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getPercY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPercY(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            if (argc == 2)
            {
                obj->setPercY(value);
            }
            else if (argc == 3)
            {
                const auto b = luaL_checkinteger(L, 3);
                obj->setPercY(value, b);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getScrollX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getScrollX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setScrollX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setScrollX(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_getScrollY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getScrollY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setScrollY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->setScrollY(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_getScrollPos(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairyvec2(L, obj->getScrollPos());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setScrollPos(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto valueX = luaL_checknumber(L, 2);
        const auto valueY = luaL_checknumber(L, 3);
        if (obj)
        {
            obj->setScrollPos(valueX, valueY);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isBottomMost(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isBottomMost());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_isRightMost(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushboolean(L, obj->isRightMost());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollLeft(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = argc >= 2 ? luaL_checknumber(L, 2) : 1.0f;
        const auto b = argc >= 3 ? luaL_checkinteger(L, 3) : false;
        if (obj)
        {
            obj->scrollLeft(value, b);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollRight(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = argc >= 2 ? luaL_checknumber(L, 2) : 1.0f;
        const auto b = argc >= 3 ? luaL_checkinteger(L, 3) : false;
        if (obj)
        {
            obj->scrollRight(value, b);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollUp(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = argc >= 2 ? luaL_checknumber(L, 2) : 1.0f;
        const auto b = argc >= 3 ? luaL_checkinteger(L, 3) : false;
        if (obj)
        {
            obj->scrollUp(value, b);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollDown(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = argc >= 2 ? luaL_checknumber(L, 2) : 1.0f;
        const auto b = argc >= 3 ? luaL_checkinteger(L, 3) : false;
        if (obj)
        {
            obj->scrollDown(value, b);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollTop(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto b = argc >= 2 ? luaL_checkinteger(L, 2) : false;
        if (obj)
        {
            obj->scrollTop(b);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollBottom(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto b = argc >= 2 ? luaL_checkinteger(L, 2) : false;
        if (obj)
        {
            obj->scrollBottom(b);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollToView(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto target = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto ani = argc >= 3 ? luaL_checkinteger(L, 3) : false;
        const auto setFirst = argc >= 4 ? luaL_checkinteger(L, 4) : false;
        if (obj)
        {
            obj->scrollToView(target, ani, setFirst);
        }
        return 0;
    }

    static int l_fairy_scrollpane_scrollToView_2(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        Rect rect;
        n3lua_checkfairyrect(L, 2, rect);
        const auto ani = argc >= 3 ? luaL_checkinteger(L, 3) : false;
        const auto setFirst = argc >= 4 ? luaL_checkinteger(L, 4) : false;
        if (obj)
        {
            obj->scrollToView(rect, ani, setFirst);
        }
        return 0;
    }

    static int l_fairy_scrollpane_isChildInView(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto child = n3lua_checkfairyclass<GObject>(L, 1, "fairy.object");
        if (obj && child)
        {
            lua_pushboolean(L, obj->isChildInView(child));
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPageX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushinteger(L, obj->getPageX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPageX(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            if (argc == 2)
            {
                obj->setPageX(value);
            }
            else if (argc == 3)
            {
                const auto b = luaL_checkinteger(L, 3);
                obj->setPageX(value, b);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getPageY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushinteger(L, obj->getPageY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_setPageY(lua_State* L)
    {
        int argc = lua_gettop(L);
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checkinteger(L, 2);
        if (obj)
        {
            if (argc == 2)
            {
                obj->setPageY(value);
            }
            else if (argc == 3)
            {
                const auto b = luaL_checkinteger(L, 3);
                obj->setPageY(value, b);
            }
        }
        return 0;
    }

    static int l_fairy_scrollpane_getScrollingPosX(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getScrollingPosX());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getScrollingPosY(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            lua_pushnumber(L, obj->getScrollingPosY());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getContentSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairysize(L, obj->getContentSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_getViewSize(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            n3lua_pushfairysize(L, obj->getViewSize());
            return 1;
        }
        return 0;
    }

    static int l_fairy_scrollpane_lockHeader(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->lockHeader(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_lockFooter(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        const auto value = luaL_checknumber(L, 2);
        if (obj)
        {
            obj->lockFooter(value);
        }
        return 0;
    }

    static int l_fairy_scrollpane_cancelDragging(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<ScrollPane>(L, 1, "fairy.scrollpane");
        if (obj)
        {
            obj->cancelDragging();
        }
        return 0;
    }

    static int l_fairy_scrollpane_getDraggingPane(lua_State* L)
    {
        const auto obj = ScrollPane::getDraggingPane();
        if (obj)
        {
            n3lua_pushfairyclass(L, obj);
            return 1;
        }
        return 0;
    }

    static const luaL_Reg scrollpane_method[] =
    {
        { "__tostring", l_fairy_scrollpane_tostring },
        { "GetOwner", l_fairy_scrollpane_getOwner },
        { "GetHeader", l_fairy_scrollpane_getHeader },
        { "GetFooter", l_fairy_scrollpane_getFooter },
        { "GetVtScrollBar", l_fairy_scrollpane_getVtScrollBar },
        { "GetHzScrollBar", l_fairy_scrollpane_getHzScrollBar },
        { "IsBouncebackEffect", l_fairy_scrollpane_isBouncebackEffect },
        { "SetBouncebackEffect", l_fairy_scrollpane_setBouncebackEffect },
        { "IsTouchEffect", l_fairy_scrollpane_isTouchEffect },
        { "SetTouchEffect", l_fairy_scrollpane_setTouchEffect },
        { "IsInertiaDisabled", l_fairy_scrollpane_isInertiaDisabled },
        { "SetInertiaDisabled", l_fairy_scrollpane_setInertiaDisabled },
        { "GetScrollStep", l_fairy_scrollpane_getScrollStep },
        { "SetScrollStep", l_fairy_scrollpane_setScrollStep },
        { "IsSnapToItem", l_fairy_scrollpane_isSnapToItem },
        { "SetSnapToItem", l_fairy_scrollpane_setSnapToItem },
        { "IsPageMode", l_fairy_scrollpane_isPageMode },
        { "SetPageMode", l_fairy_scrollpane_setPageMode },
        { "GetPageController", l_fairy_scrollpane_getPageController },
        { "SetPageController", l_fairy_scrollpane_setPageController },
        { "IsMouseWheelEnabled", l_fairy_scrollpane_isMouseWheelEnabled },
        { "SetMouseWheelEnabled", l_fairy_scrollpane_setMouseWheelEnabled },
        { "GetDecelerationRate", l_fairy_scrollpane_getDecelerationRate },
        { "SetDecelerationRate", l_fairy_scrollpane_setDecelerationRate },
        { "GetPosX", l_fairy_scrollpane_getPosX },
        { "SetPosX", l_fairy_scrollpane_setPosX },
        { "GetPosY", l_fairy_scrollpane_getPosY },
        { "SetPosY", l_fairy_scrollpane_setPosY },
        { "GetPos", l_fairy_scrollpane_getPos },
        { "SetPos", l_fairy_scrollpane_setPos },
        { "GetPercX", l_fairy_scrollpane_getPercX },
        { "SetPercX", l_fairy_scrollpane_setPercX },
        { "GetPercY", l_fairy_scrollpane_getPercY },
        { "SetPercY", l_fairy_scrollpane_setPercY },
        { "GetScrollX", l_fairy_scrollpane_getScrollX },
        { "SetScrollX", l_fairy_scrollpane_setScrollX },
        { "GetScrollY", l_fairy_scrollpane_getScrollY },
        { "SetScrollY", l_fairy_scrollpane_setScrollY },
        { "GetScrollPos", l_fairy_scrollpane_getScrollPos },
        { "SetScrollPos", l_fairy_scrollpane_setScrollPos },
        { "IsBottomMost", l_fairy_scrollpane_isBottomMost },
        { "IsRightMost", l_fairy_scrollpane_isRightMost },
        { "ScrollLeft", l_fairy_scrollpane_scrollLeft },
        { "ScrollRight", l_fairy_scrollpane_scrollRight },
        { "ScrollUp", l_fairy_scrollpane_scrollUp },
        { "ScrollDown", l_fairy_scrollpane_scrollDown },
        { "ScrollTop", l_fairy_scrollpane_scrollTop },
        { "ScrollBottom", l_fairy_scrollpane_scrollBottom },
        { "ScrollToView", l_fairy_scrollpane_scrollToView },
        { "ScrollToView_2", l_fairy_scrollpane_scrollToView_2 },
        { "IsChildInView", l_fairy_scrollpane_isChildInView },
        { "GetPageX", l_fairy_scrollpane_getPageX },
        { "SetPageX", l_fairy_scrollpane_setPageX },
        { "GetPageY", l_fairy_scrollpane_getPageY },
        { "SetPageY", l_fairy_scrollpane_setPageY },
        { "GetScrollingPosX", l_fairy_scrollpane_getScrollingPosX },
        { "GetScrollingPosY", l_fairy_scrollpane_getScrollingPosY },
        { "GetContentSize", l_fairy_scrollpane_getContentSize },
        { "GetViewSize", l_fairy_scrollpane_getViewSize },
        { "LockHeader", l_fairy_scrollpane_lockHeader },
        { "LockFooter", l_fairy_scrollpane_lockFooter },
        { "CancelDragging", l_fairy_scrollpane_cancelDragging },
        { "GetDraggingPane", l_fairy_scrollpane_getDraggingPane },
        { nullptr, nullptr }
    };
}


        