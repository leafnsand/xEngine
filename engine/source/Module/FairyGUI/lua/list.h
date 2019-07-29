#pragma once

#include "ui/GList.h"

#include "util.h"

using namespace fairygui;

namespace N3Lua
{
    static int l_fairy_list_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.list");
        return 1;
    }

    static int l_fairy_list_GetDefaultItem(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushstring(L, list->getDefaultItem().c_str());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetDefaultItem(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto* item = luaL_checkstring(L, 2);
            list->setDefaultItem(item);
        }
        return 0;
    }

    static int l_fairy_list_GetLayout(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, int(list->getLayout()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetLayout(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto layout = luaL_checkinteger(L, 2);
            list->setLayout(ListLayoutType(layout));
        }
        return 0;
    }

    static int l_fairy_list_GetLineCount(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, list->getLineCount());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetLineCount(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto count = luaL_checkinteger(L, 2);
            list->setLineCount(count);
        }
        return 0;
    }

    static int l_fairy_list_GetColumnCount(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, list->getColumnCount());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetColumnCount(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto count = luaL_checkinteger(L, 2);
            list->setColumnCount(count);
        }
        return 0;
    }

    static int l_fairy_list_GetColumnGap(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, list->getColumnGap());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetColumnGap(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto gap = luaL_checkinteger(L, 2);
            list->setColumnGap(gap);
        }
        return 0;
    }

    static int l_fairy_list_GetLineGap(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, list->getLineGap());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetLineGap(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto gap = luaL_checkinteger(L, 2);
            list->setLineGap(gap);
        }
        return 0;
    }

    static int l_fairy_list_GetAlign(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, int(list->getAlign()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetAlign(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto align = luaL_checkinteger(L, 2);
            list->setAlign(TextHAlignment(align));
        }
        return 0;
    }

    static int l_fairy_list_GetVerticalAlign(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, int(list->getVerticalAlign()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetVerticalAlign(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto align = luaL_checkinteger(L, 2);
            list->setVerticalAlign(TextVAlignment(align));
        }
        return 0;
    }

    static int l_fairy_list_GetAutoResizeItem(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushboolean(L, list->getAutoResizeItem());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetAutoResizeItem(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto value = luaL_checkinteger(L, 2);
            list->setAutoResizeItem(value);
        }
        return 0;
    }

    static int l_fairy_list_GetSelectionMode(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, int(list->getSelectionMode()));
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetSelectionMode(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto value = luaL_checkinteger(L, 2);
            list->setSelectionMode(ListSelectionMode(value));
        }
        return 0;
    }

    static int l_fairy_list_GetItemPool(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
//            list->getItemPool();
        }
        return 0;
    }

    static int l_fairy_list_GetFromPool(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");
        
        if (list)
        {
//            list->getFromPool();
        }
        return 0;
    }

    static int l_fairy_list_ReturnToPool(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");
        
        if (list)
        {
//            list->returnToPool();
        }
        return 0;
    }

    static int l_fairy_list_AddItemFromPool(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");
        
        if (list)
        {
//            list->addItemFromPool();
        }
        return 0;
    }

    static int l_fairy_list_AddChildAt(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
            if (child)
            {
                const auto index = luaL_checkinteger(L, 3);
                list->addChildAt(child, index);
            }
        }
        return 0;
    }

    static int l_fairy_list_RemoveChildAt(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto index = luaL_checkinteger(L, 2);
            list->removeChildAt(index);
        }
        return 0;
    }

    static int l_fairy_list_RemoveChildToPoolAt(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto index = luaL_checkinteger(L, 2);
            list->removeChildToPoolAt(index);
        }
        return 0;
    }

    static int l_fairy_list_RemoveChildToPool(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto child = n3lua_checkfairyclass<GObject>(L, 2, "fairy.object");
            if (child)
            {
                list->removeChildToPool(child);
            }
        }
        return 0;
    }

    static int l_fairy_list_RemoveChildrenToPool(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->removeChildrenToPool();
        }
        return 0;
    }

    static int l_fairy_list_GetSelectedIndex(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L,list->getSelectedIndex());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetSelectedIndex(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            auto const index = luaL_checkinteger(L, 2);
            list->setSelectedIndex(index);
        }
        return 0;
    }

    static int l_fairy_list_GetSelection(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            AZStd::vector<int> result;
            list->getSelection(result);
            lua_createtable(L, int(result.size()), 0);
            for (int i = 0; i < result.size(); ++i)
            {
                lua_pushinteger(L, result[i]);
                lua_rawseti(L, -2, i + 1);
            }
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_AddSelection(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            auto const index = luaL_checkinteger(L, 2);
            auto const value = luaL_checkinteger(L, 3);
            list->addSelection(index, value);
        }
        return 0;
    }

    static int l_fairy_list_RemoveSelection(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            auto const index = luaL_checkinteger(L, 2);
            list->removeSelection(index);
        }
        return 0;
    }

    static int l_fairy_list_ClearSelection(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->clearSelection();
        }
        return 0;
    }

    static int l_fairy_list_SelectAll(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->selectAll();
        }
        return 0;
    }

    static int l_fairy_list_SelectReverse(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->selectReverse();
        }
        return 0;
    }

    static int l_fairy_list_HandleArrowKey(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            auto const dir = luaL_checkinteger(L, 2);
            list->handleArrowKey(dir);
        }
        return 0;
    }

    static int l_fairy_list_ResizeToFit(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            auto const count = luaL_checkinteger(L, 2);
            auto const min = luaL_optinteger(L, 3, 0);
            list->resizeToFit(count, min);
        }
        return 0;
    }

    static int l_fairy_list_GetFirstChildInView(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, list->getFirstChildInView());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_ScrollToView(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            auto const index = luaL_checkinteger(L, 2);
            auto const ani = luaL_optinteger(L, 3, 0);
            auto const setFirst = luaL_optinteger(L, 4, 0);
            list->scrollToView(index, ani, setFirst);
        }
        return 0;
    }

    static int l_fairy_list_GetSelectionController(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            n3lua_pushfairyclass(L, list->getSelectionController());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetSelectionController(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");
        const auto controller = n3lua_checkfairyclass<GController>(L, 2, "fairy.controller");
        if (list && controller)
        {
             list->setSelectionController(controller);
        }
        return 0;
    }

    static int l_fairy_list_SetVirtual(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->setVirtual();
        }
        return 0;
    }

    static int l_fairy_list_SetVirtualAndLoop(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->setVirtualAndLoop();
        }
        return 0;
    }

    static int l_fairy_list_IsVirtual(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushboolean(L, list->isVirtual());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_RefreshVirtualList(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            list->refreshVirtualList();
        }
        return 0;
    }

    static int l_fairy_list_GetNumItems(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            lua_pushinteger(L, list->getNumItems());
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetNumItems(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto value = luaL_checkinteger(L, 2);
            list->setNumItems(value);
        }
        return 0;
    }

    static int l_fairy_list_ChildIndexToItemIndex(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto value = luaL_checkinteger(L, 2);
            lua_pushinteger(L, list->childIndexToItemIndex(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_ItemIndexToChildIndex(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            const auto value = luaL_checkinteger(L, 2);
            lua_pushinteger(L, list->itemIndexToChildIndex(value));
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_GetSnappingPosition(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");        
        if (list)
        {
            Vec2 vec;
            n3lua_checkfairyvec2(L, 2, vec);
            const auto result = list->getSnappingPosition(vec);
            n3lua_pushfairyvec2(L, result);
            return 1;
        }
        return 0;
    }

    static int l_fairy_list_SetItemRenderer(lua_State* L)
    {
        const auto list = n3lua_checkfairyclass<GList>(L, 1, "fairy.list");
        if (list)
        {
            LuaRefCallBack callback(L, 2);
            list->setItemRenderer([L, callback](int index, GObject* object)
            {
                lua_pushinteger(L, index);
                n3lua_pushfairyclass(L, object);
                callback.call(2);
            });
        }
        return 0;
    }
    
    static const luaL_Reg list_method[] =
    {
        { "__tostring", l_fairy_list_tostring },
        { "GetDefaultItem", l_fairy_list_GetDefaultItem },
        { "SetDefaultItem", l_fairy_list_SetDefaultItem },
        { "GetLayout", l_fairy_list_GetLayout },
        { "SetLayout", l_fairy_list_SetLayout },
        { "GetLineCount", l_fairy_list_GetLineCount },
        { "SetLineCount", l_fairy_list_SetLineCount },
        { "GetColumnCount", l_fairy_list_GetColumnCount },
        { "SetColumnCount", l_fairy_list_SetColumnCount },
        { "GetColumnGap", l_fairy_list_GetColumnGap },
        { "SetColumnGap", l_fairy_list_SetColumnGap },
        { "GetLineGap", l_fairy_list_GetLineGap },
        { "SetLineGap", l_fairy_list_SetLineGap },
        { "GetAlign", l_fairy_list_GetAlign },
        { "SetAlign", l_fairy_list_SetAlign },
        { "GetVerticalAlign", l_fairy_list_GetVerticalAlign },
        { "SetVerticalAlign", l_fairy_list_SetVerticalAlign },
        { "GetAutoResizeItem", l_fairy_list_GetAutoResizeItem },
        { "SetAutoResizeItem", l_fairy_list_SetAutoResizeItem },
        { "GetSelectionMode", l_fairy_list_GetSelectionMode },
        { "SetSelectionMode", l_fairy_list_SetSelectionMode },
        { "GetItemPool", l_fairy_list_GetItemPool },
        { "GetFromPool", l_fairy_list_GetFromPool },
        { "ReturnToPool", l_fairy_list_ReturnToPool },
        { "AddItemFromPool", l_fairy_list_AddItemFromPool },
        { "AddChildAt", l_fairy_list_AddChildAt },
        { "RemoveChildAt", l_fairy_list_RemoveChildAt },
        { "RemoveChildToPoolAt", l_fairy_list_RemoveChildToPoolAt },
        { "RemoveChildToPool", l_fairy_list_RemoveChildToPool },
        { "RemoveChildrenToPool", l_fairy_list_RemoveChildrenToPool },
        { "GetSelectedIndex", l_fairy_list_GetSelectedIndex },
        { "SetSelectedIndex", l_fairy_list_SetSelectedIndex },
        { "GetSelection", l_fairy_list_GetSelection },
        { "AddSelection", l_fairy_list_AddSelection },
        { "RemoveSelection", l_fairy_list_RemoveSelection },
        { "ClearSelection", l_fairy_list_ClearSelection },
        { "SelectAll", l_fairy_list_SelectAll },
        { "SelectReverse", l_fairy_list_SelectReverse },
        { "HandleArrowKey", l_fairy_list_HandleArrowKey },
        { "ResizeToFit", l_fairy_list_ResizeToFit },
        { "GetFirstChildInView", l_fairy_list_GetFirstChildInView },
        { "ScrollToView", l_fairy_list_ScrollToView },
        { "GetSelectionController", l_fairy_list_GetSelectionController },
        { "SetSelectionController", l_fairy_list_SetSelectionController },
        { "SetVirtual", l_fairy_list_SetVirtual },
        { "SetVirtualAndLoop", l_fairy_list_SetVirtualAndLoop },
        { "IsVirtual", l_fairy_list_IsVirtual },
        { "RefreshVirtualList", l_fairy_list_RefreshVirtualList },
        { "GetNumItems", l_fairy_list_GetNumItems },
        { "SetNumItems", l_fairy_list_SetNumItems },
        { "ChildIndexToItemIndex", l_fairy_list_ChildIndexToItemIndex },
        { "ItemIndexToChildIndex", l_fairy_list_ItemIndexToChildIndex },
        { "GetSnappingPosition", l_fairy_list_GetSnappingPosition },
        { "SetItemRenderer", l_fairy_list_SetItemRenderer },
        { nullptr, nullptr }
    };
}