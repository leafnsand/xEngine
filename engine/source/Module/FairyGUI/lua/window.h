#pragma once

#include "n3luautil.h"
#include "FairyGUI/fairy/FairyGUI.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_window_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.window");
        return 1;
    }

    static int l_fairy_window_Show(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->show();
        }
        return 0;
    }

    static int l_fairy_window_Hide(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->hide();
        }
        return 0;
    }

    static int l_fairy_window_HideImmediately(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->hideImmediately();
        }
        return 0;
    }

    static int l_fairy_window_ToggleStatus(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->toggleStatus();
        }
        return 0;
    }

    static int l_fairy_window_BringToFront(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->bringToFront();
        }
        return 0;
    }

    static int l_fairy_window_IsShowing(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            lua_pushboolean(L, obj->isShowing());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_IsTop(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            lua_pushboolean(L, obj->isTop());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_IsModal(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            lua_pushboolean(L, obj->isModal());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_SetModal(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        bool value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setModal(value);
        }
        return 0;
    }

    static int l_fairy_window_ShowModalWait(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            const auto cmd = luaL_optinteger(L, 2, 0);
            obj->showModalWait(cmd);
        }
        return 0;
    }

    static int l_fairy_window_CloseModalWait(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->closeModalWait();
        }
        return 0;
    }

    static int l_fairy_window_InitWindow(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            obj->initWindow();
        }
        return 0;
    }

    static int l_fairy_window_AddUISource(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        const auto uisource = n3lua_checkfairyclass<IUISource>(L, 1, "fairy.uisource");
        if (obj && uisource)
        {
            obj->addUISource(uisource);
        }
        return 0;
    }

    static int l_fairy_window_IsBringToFrontOnClick(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            lua_toboolean(L, obj->isBringToFrontOnClick());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_SetBringToFrontOnClick(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        bool value = lua_toboolean(L, 2) == 1;        
        if (obj)
        {
            obj->setBringToFrontOnClick(value);
        }
        return 0;
    }

    static int l_fairy_window_GetContentPane(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getContentPane());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_SetContentPane(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        const auto panel = n3lua_checkfairyclass<GComponent>(L, 1, "fairy.component");
        if (obj && panel)
        {
            obj->setContentPane(panel);
        }
        return 0;
    }

    static int l_fairy_window_GetFrame(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getFrame());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_GetCloseButton(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getCloseButton());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_SetCloseButton(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        const auto value = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");        
        if (obj && value)
        {
            obj->setCloseButton(value);
        }
        return 0;
    }

    static int l_fairy_window_GetDragArea(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getDragArea());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_SetDragArea(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        const auto value = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (obj && value)
        {
            obj->setDragArea(value);
        }
        return 0;
    }

    static int l_fairy_window_GetContentArea(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getContentArea());
            return 1;
        }
        return 0;
    }

    static int l_fairy_window_SetContentArea(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");
        const auto value = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (obj && value)
        {
            obj->setContentArea(value);
        }
        return 0;
    }

    static int l_fairy_window_GetModalWaitingPane(lua_State* L)
    {
        const auto obj = n3lua_checkfairyclass<Window>(L, 1, "fairy.window");        
        if (obj)
        {
            n3lua_pushfairyclass(L, obj->getModalWaitingPane());
            return 1;
        }
        return 0;
    }

    static const luaL_Reg window_method[] =
    {
        { "__tostring", l_fairy_window_tostring },
        { "Show", l_fairy_window_Show },
        { "Hide", l_fairy_window_Hide },
        { "HideImmediately", l_fairy_window_HideImmediately },
        { "ToggleStatus", l_fairy_window_ToggleStatus },
        { "BringToFront", l_fairy_window_BringToFront },
        { "IsShowing", l_fairy_window_IsShowing },
        { "IsTop", l_fairy_window_IsTop },
        { "IsModal", l_fairy_window_IsModal },
        { "SetModal", l_fairy_window_SetModal },
        { "ShowModalWait", l_fairy_window_ShowModalWait },
        { "CloseModalWait", l_fairy_window_CloseModalWait },
        { "InitWindow", l_fairy_window_InitWindow },
        { "AddUISource", l_fairy_window_AddUISource },
        { "IsBringToFrontOnClick", l_fairy_window_IsBringToFrontOnClick },
        { "SetBringToFrontOnClick", l_fairy_window_SetBringToFrontOnClick },
        { "GetContentPane", l_fairy_window_GetContentPane },
        { "SetContentPane", l_fairy_window_SetContentPane },
        { "GetFrame", l_fairy_window_GetFrame },
        { "GetCloseButton", l_fairy_window_GetCloseButton },
        { "SetCloseButton", l_fairy_window_SetCloseButton },
        { "GetDragArea", l_fairy_window_GetDragArea },
        { "SetDragArea", l_fairy_window_SetDragArea },
        { "GetContentArea", l_fairy_window_GetContentArea },
        { "SetContentArea", l_fairy_window_SetContentArea },
        { "GetModalWaitingPane", l_fairy_window_GetModalWaitingPane },
        { nullptr, nullptr }
    };
}