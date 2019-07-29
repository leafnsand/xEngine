#if __WIN32__
#pragma warning(disable: 4244)
#endif

#include "basictextfield.h"
#include "button.h"
#include "combobox.h"
#include "component.h"
#include "graph.h"
#include "group.h"
#include "image.h"
#include "label.h"
#include "list.h"
#include "loader.h"
#include "movieclip.h"
#include "object.h"
#include "package.h"
#include "progressbar.h"
#include "richtextfield.h"
#include "root.h"
#include "scrollbar.h"
#include "slider.h"
#include "textfield.h"
#include "textinput.h"
#include "window.h"
#include "uipanel.h"

#include "controller.h"
#include "objectpool.h"
#include "popupmenu.h"
#include "scrollpane.h"
#include "uieventdispatcher.h"
#include "inputevent.h"
#include "eventcontext.h"
#include "relations.h"
#include "transition.h"

#include "spine.h"
#include "particle2d.h"

#include "AzCore/EBus/EBus.h"
#include "EBus/FairySystemComponentBus.h"
#include "AzCore/Component/GameEntityContextBus.h"

#include <AzCore/Script/lua/lua.h>

#define FAIRY_SUB_LIB(L, name, table) { \
    luaL_newlib(L, table); \
    lua_setfield(L, -2, name); \
}

namespace N3Lua
{
    static int l_fairy_AddPackage(lua_State* L)
    {
        const auto path = luaL_checkstring(L, 1);
        LuaRefCallBack callback(L, 2);
        EBUS_EVENT(Module::FairyPackageSystemRequestBus, AddPackage, path, [callback]()
        {
            callback.call();
        });
        return 0;
    }

    static int l_fairy_GetPackageItemURL(lua_State* L)
    {
        const auto packageName = luaL_checkstring(L, 1);
        const auto packageItemName = luaL_checkstring(L, 2);
        AZStd::string ItemUrl;
        EBUS_EVENT_RESULT(ItemUrl,Module::FairyPackageSystemRequestBus, GetPackageItemURL, packageName, packageItemName);
        lua_pushstring(L, ItemUrl.c_str());
        return 1;
    }

    static int l_fairy_DestroyUIPanel(lua_State* L)
    {
        auto& id = n3lua_checkclass<AZ::EntityId>(L, 1, "UIPanel");
        if (id.IsValid())
        {
            EBUS_EVENT(AZ::GameEntityContextRequestBus, DestroyGameEntity, id);
        }
        return 0;
    }

    static int l_fairy_CreateUIPanel(lua_State* L)
    {
        AZ::EntityId uiPanelId;
        int argc = lua_gettop(L);
        if (argc == 2)
        {
            const auto pkgName = luaL_checkstring(L, 1);
            const auto componentName = luaL_checkstring(L, 2);
            EBUS_EVENT_RESULT(uiPanelId, Module::FairyPackageSystemRequestBus, CreateUIPanel, pkgName, componentName);
        }
        else
        {
            EBUS_EVENT_RESULT(uiPanelId, Module::FairyPackageSystemRequestBus, CreateUIPanel,"","");
        }
        n3lua_pushclass(L, "UIPanel", uiPanelId, l_fairy_DestroyUIPanel);
        return 1;
    }

    static int l_fairy_CreateObject(lua_State* L)
    {
        const auto pkgname = luaL_checkstring(L, 1);
        const auto itemname = luaL_checkstring(L, 2);
        GObject* gobject;
        EBUS_EVENT_RESULT(gobject, Module::FairyPackageSystemRequestBus, CreateObject, pkgname, itemname);
        n3lua_pushfairyclass(L, gobject);
        return 1;
    }


    static int l_fairy_CreateEmptyObject(lua_State* L)
    {
        const auto type = luaL_checkstring(L, 1);
        Ref* emptyItem;
        EBUS_EVENT_RESULT(emptyItem, Module::FairyPackageSystemRequestBus, CreateEmptyObject, type);
        n3lua_pushfairyclass(L, emptyItem);
        return 1;
    }
    
    static int l_fairy_CreateSpine(lua_State* L)
    {
        const auto name = luaL_checkstring(L, 1);
        // n3lua_pushfairyclass(L, FairyFacade::Instance()->CreateSpine(name));
        // return 1;
        return 0;
    }

    static int l_fairy_CreateParticle2d(lua_State* L)
    {
        const auto name = luaL_checkstring(L, 1);
        // n3lua_pushfairyclass(L, FairyFacade::Instance()->CreateParticle2d(name));
        // return 1;
        return 0;
    }    

    static int l_fairy_GetMetatable(lua_State* L)
    {
        const auto name = luaL_checkstring(L, 1);
        luaL_getmetatable(L, name);
        return 1;
    }

    static int l_fairy_Root(lua_State* L)
    {
        GRoot* rootPtr;
        EBUS_EVENT_RESULT(rootPtr, Module::FairyPackageSystemRequestBus, GetGRoot);
        n3lua_pushfairyclass(L, rootPtr);
        return 1;
    }

    static int l_fairy_ref_tostring(lua_State* L)
    {
        lua_pushstring(L, "fairy.ref");
        return 1;
    }

    static const luaL_Reg ref_method[] =
    {
        { "__tostring", l_fairy_ref_tostring },
        { nullptr, nullptr }
    };

    static const luaL_Reg fairylib[] =
    {
        { "AddPackage", l_fairy_AddPackage },
        { "GetPackageItemURL", l_fairy_GetPackageItemURL },
        { "CreateUIPanel", l_fairy_CreateUIPanel},
        { "CreateObject", l_fairy_CreateObject },
        { "CreateEmptyObject", l_fairy_CreateEmptyObject },
        { "CreateSpine", l_fairy_CreateSpine },
        { "CreateParticle2d", l_fairy_CreateParticle2d },
        { "GetMetatable", l_fairy_GetMetatable },
        { "Root", l_fairy_Root },
        { nullptr, nullptr }
    };

    int luaopen_fairy(lua_State* L)
    {
        luaL_newlib(L, fairylib);
        // luaL_openlibs(L);
        return 1;
    }

    const char* overrideIndex = 
        "_global_index = function(t, k)\n"
            "local m = getmetatable(t)\n"
            "if m and m._override_index then\n"
                "return m._override_index(t, k, m)"
            "end\n"
            "return nil\n"
        "end\n"
        "_global_newindex = function(t, k, v)\n"
            "local m = getmetatable(t)\n"
            "if m and m._override_newindex then\n"
                "m._override_newindex(t, k, v, m)\n"
            "end\n"
        "end\n"
        "_global_override_index = function(t, k, m)\n"
            "if global_override_index then\n"
                "local ret = global_override_index(t, k)\n"
                "if ret ~= nil then return ret end\n"
            "end\n"
            "if m[k] then return m[k] end\n"
            "m = getmetatable(m)\n"
            "if m and m._override_index then\n"
                "return m._override_index(t, k, m)\n"
            "end\n"
        "end\n"
        "_global_override_newindex = function(t, k, v, m)\n"
            "if global_override_newindex then\n"
                "local ret = global_override_newindex(t, k, v)\n"
                "if ret == true then return end\n"
            "end\n"
            "if not m[k] then m[k] = v end\n"
        "end\n";

    void UseFairy(lua_State* L)
    {
        luaL_dostring(L, overrideIndex);

        n3lua_addclass(L, "fairy.ref", ref_method);
        n3lua_addclass(L, "fairy.uieventdispatcher", uieventdispatcher_method, "fairy.ref");
        n3lua_addclass(L, "fairy.package", package_method, "fairy.ref");
        n3lua_addclass(L, "fairy.object", object_method, "fairy.uieventdispatcher");
        n3lua_addclass(L, "fairy.component", component_method, "fairy.object");
        n3lua_addclass(L, "fairy.button", button_method, "fairy.component");
        n3lua_addclass(L, "fairy.combobox", combobox_method, "fairy.component");
        n3lua_addclass(L, "fairy.label", label_method, "fairy.component");
        n3lua_addclass(L, "fairy.list", list_method, "fairy.component");
        n3lua_addclass(L, "fairy.progressbar", progressbar_method, "fairy.component");
        n3lua_addclass(L, "fairy.root", root_method, "fairy.component");
        n3lua_addclass(L, "fairy.scrollbar", scrollbar_method, "fairy.component");
        n3lua_addclass(L, "fairy.slider", slider_method, "fairy.component");
        n3lua_addclass(L, "fairy.window", window_method, "fairy.component");
        n3lua_addclass(L, "fairy.graph", graph_method, "fairy.object");
        n3lua_addclass(L, "fairy.group", group_method, "fairy.object");
        n3lua_addclass(L, "fairy.image", image_method, "fairy.object");
        n3lua_addclass(L, "fairy.loader", loader_method, "fairy.object");
        n3lua_addclass(L, "fairy.movieclip", movieclip_method, "fairy.object");
        n3lua_addclass(L, "fairy.textfield", textfield_method, "fairy.object");
        n3lua_addclass(L, "fairy.basictextfield", basictextfield_method, "fairy.textfield");
        n3lua_addclass(L, "fairy.richtextfield", richtextfield_method, "fairy.textfield");
        n3lua_addclass(L, "fairy.textinput", textinput_method, "fairy.textfield");
        n3lua_addclass(L, "fairy.spine", spine_method, "fairy.object");
        n3lua_addclass(L, "fairy.particle2d", particle2d_method, "fairy.object");
        n3lua_addclass(L, "fairy.controller", controller_method, "fairy.uieventdispatcher");
        n3lua_addclass(L, "fairy.objectpool", objectpool_method);
        n3lua_addclass(L, "fairy.popupmenu", popupmenu_method, "fairy.ref");
        n3lua_addclass(L, "fairy.scrollpane", scrollpane_method, "fairy.ref");
        n3lua_addclass(L, "fairy.inputevent", inputevent_method, "fairy.ref");
        n3lua_addclass(L, "fairy.eventcontext", eventcontext_method, "fairy.ref");
        n3lua_addclass(L, "fairy.relations", relations_method, "fairy.ref");
        n3lua_addclass(L, "fairy.transition", transition_method, "fairy.ref");
        n3lua_addclass(L, "UIPanel", uiPanel_method);

        luaL_requiref(L, "fairy", luaopen_fairy, 1);
        lua_pop(L, 1);
    }
} // namespace N3Lua
