#pragma once


#include "util.h"

#include "ui/GRoot.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_root_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.root");
        return 1;
    }

    static int l_fairy_root_ShowWindow(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto window = n3lua_checkfairyclass<Window>(L, 2, "fairy.window");
        if (root && window)
        {
            root->showWindow(window);
        }
        return 0;
    }

    static int l_fairy_root_HideWindow(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto window = n3lua_checkfairyclass<Window>(L, 2, "fairy.window");
        if (root && window)
        {
            root->hideWindow(window);
        }
        return 0;
    }

    static int l_fairy_root_HideWindowImmediately(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto window = n3lua_checkfairyclass<Window>(L, 2, "fairy.window");
        if (root && window)
        {
            root->hideWindowImmediately(window);
        }
        return 0;
    }

    static int l_fairy_root_BringToFront(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto window = n3lua_checkfairyclass<Window>(L, 2, "fairy.window");
        if (root && window)
        {
            root->bringToFront(window);
        }
        return 0;
    }

    static int l_fairy_root_ShowModalWait(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            root->showModalWait();
        }
        return 0;
    }

    static int l_fairy_root_CloseModalWait(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            root->closeModalWait();
        }
        return 0;
    }

    static int l_fairy_root_CloseAllExceptModals(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            root->closeAllExceptModals();
        }
        return 0;
    }

    static int l_fairy_root_CloseAllWindows(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            root->closeAllWindows();
        }
        return 0;
    }

    static int l_fairy_root_GetTopWindow(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            const auto window = root->getTopWindow();
            n3lua_pushfairyclass(L, window);
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_GetModalWaitingPane(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            const auto pane = root->getModalWaitingPane();
            n3lua_pushfairyclass(L, pane);
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_GetModalLayer(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            const auto graph = root->getModalLayer();
            n3lua_pushfairyclass(L, graph);
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_HasModalWindow(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            lua_pushboolean(L, root->hasModalWindow());
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_IsModalWaiting(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            lua_pushboolean(L, root->isModalWaiting());
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_GetTouchPosition(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto touchId = luaL_checkinteger(L, 2);
        if (root)
        {
            const auto& vec = root->getTouchPosition(touchId);
            lua_pushnumber(L, vec.x);
            lua_pushnumber(L, vec.y);
            return 2;
        }
        return 0;
    }

    static int l_fairy_root_GetTouchTarget(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            n3lua_pushfairyclass(L, root->getTouchTarget());
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_ShowPopup(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto popup = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (root && popup)
        {
            root->showPopup(popup);
        }
        return 0;
    }

    static int l_fairy_root_TogglePopup(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto popup = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (root && popup)
        {
            root->togglePopup(popup);
        }
        return 0;
    }

    static int l_fairy_root_HidePopup(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            root->hidePopup();
        }
        return 0;
    }

    static int l_fairy_root_HasAnyPopup(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            lua_pushboolean(L, root->hasAnyPopup());
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_GetPoupPosition(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto popup = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        const auto target = n3lua_checkfairyclass<GObject>(L, 3, "fairy.object");
        const auto dir = luaL_checkinteger(L, 4);
        if (root)
        {
            const auto vec = root->getPoupPosition(popup, target, PopupDirection(dir));
            n3lua_pushfairyvec2(L, vec);
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_ShowTooltips(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto tips = luaL_checkstring(L, 2);
        if (root)
        {
            root->showTooltips(tips);
        }
        return 0;
    }

    static int l_fairy_root_ShowTooltipsWin(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto tips = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
        if (root && tips)
        {
            root->showTooltipsWin(tips);
        }
        return 0;
    }

    static int l_fairy_root_HideTooltips(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            root->hideTooltips();
        }
        return 0;
    }

    static int l_fairy_root_PlaySound(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto name = luaL_checkstring(L, 2);
        const auto value = luaL_optnumber(L, 2, 1.0f);
        if (root)
        {
            root->playSound(name, value);
        }
        return 0;
    }

    static int l_fairy_root_IsSoundEnabled(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            lua_pushboolean(L, root->isSoundEnabled());
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_SetSoundEnabled(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto value = luaL_checkinteger(L, 2) == 1;
        if (root)
        {
            root->setSoundEnabled(value);
        }
        return 0;
    }

    static int l_fairy_root_GetSoundVolumeScale(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        if (root)
        {
            lua_pushnumber(L, root->getSoundVolumeScale());
            return 1;
        }
        return 0;
    }

    static int l_fairy_root_SetSoundVolumeScale(lua_State* L)
    {
        const auto root = n3lua_checkfairyclass<GRoot>(L, 1, "fairy.root");
        const auto value = luaL_checknumber(L, 2);
        if (root)
        {
            root->setSoundVolumeScale(value);
        }
        return 0;
    }

    static const luaL_Reg root_method[] =
    {
        { "__tostring", l_fairy_root_tostring },
        { "ShowWindow", l_fairy_root_ShowWindow },
        { "HideWindow", l_fairy_root_HideWindow },
        { "HideWindowImmediately", l_fairy_root_HideWindowImmediately },
        { "BringToFront", l_fairy_root_BringToFront },
        { "ShowModalWait", l_fairy_root_ShowModalWait },
        { "CloseModalWait", l_fairy_root_CloseModalWait },
        { "CloseAllExceptModals", l_fairy_root_CloseAllExceptModals },
        { "CloseAllWindows", l_fairy_root_CloseAllWindows },
        { "GetTopWindow", l_fairy_root_GetTopWindow },
        { "GetModalWaitingPane", l_fairy_root_GetModalWaitingPane },
        { "GetModalLayer", l_fairy_root_GetModalLayer },
        { "HasModalWindow", l_fairy_root_HasModalWindow },
        { "IsModalWaiting", l_fairy_root_IsModalWaiting },
        { "GetTouchPosition", l_fairy_root_GetTouchPosition },
        { "GetTouchTarget", l_fairy_root_GetTouchTarget },
        { "ShowPopup", l_fairy_root_ShowPopup },
        { "TogglePopup", l_fairy_root_TogglePopup },
        { "HidePopup", l_fairy_root_HidePopup },
        { "HasAnyPopup", l_fairy_root_HasAnyPopup },
        { "GetPoupPosition", l_fairy_root_GetPoupPosition },
        { "ShowTooltips", l_fairy_root_ShowTooltips },
        { "ShowTooltipsWin", l_fairy_root_ShowTooltipsWin },
        { "HideTooltips", l_fairy_root_HideTooltips },
        { "PlaySound", l_fairy_root_PlaySound },
        { "IsSoundEnabled", l_fairy_root_IsSoundEnabled },
        { "SetSoundEnabled", l_fairy_root_SetSoundEnabled },
        { "GetSoundVolumeScale", l_fairy_root_GetSoundVolumeScale },
        { "SetSoundVolumeScale", l_fairy_root_SetSoundVolumeScale },
        { nullptr, nullptr }
    };
}