/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#ifndef AZ_UNITY_BUILD

#if !defined(AZCORE_EXCLUDE_LUA)

#include <AzCore/Script/ScriptContext.h>
#include <AzCore/Script/ScriptSystemBus.h>

#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <AzCore/std/string/conversions.h>

#include <AzCore/Script/ScriptComponent.h>

#include <AzCore/StringFunc/StringFunc.h>

extern "C" {
#   include <lua/lualib.h>
#   include <lua/lauxlib.h>
}

namespace AZ
{
    //=========================================================================
    // ScriptComponent
    // [8/9/2013]
    //=========================================================================
    ScriptComponent::ScriptComponent()
        : m_context(nullptr)
        , m_script(static_cast<AZ::u8>(AZ::Data::AssetFlags::OBJECTSTREAM_PRE_LOAD))
        , m_table(LUA_NOREF)
    {
    }

    //=========================================================================
    // ~PlacementComponent
    // [8/9/2013]
    //=========================================================================
    ScriptComponent::~ScriptComponent()
    {
    }

    //=========================================================================
    // SetScriptContext
    // [8/9/2013]
    //=========================================================================
    void ScriptComponent::SetScriptContext(AZ::ScriptContext* context)
    {
        AZ_Assert(m_entity == nullptr || m_entity->GetState() != AZ::Entity::ES_ACTIVE, "You can't change the context while the entity is active");
        m_context = context;
    }

    //=========================================================================
    // SetScriptName
    // [8/9/2013]
    //=========================================================================
    void ScriptComponent::SetScript(const AZ::Data::Asset<AZ::ScriptAsset>& script)
    {
        AZ_Assert(m_entity == nullptr || m_entity->GetState() != AZ::Entity::ES_ACTIVE, "You can't change the script while the entity is active");

        m_script = script;
    }

    void ScriptComponent::Init()
    {
        // Grab the script context
        EBUS_EVENT_RESULT(m_context, AZ::ScriptSystemRequestBus, GetContext);
        AZ_Assert(m_context, "We must have a valid script context!");
    }

    //=========================================================================
    // Activate
    // [8/12/2013]
    //=========================================================================
    void ScriptComponent::Activate()
    {
        AZ::ScriptSystemNotificationBus::Handler::BusConnect();
    }

    //=========================================================================
    // Deactivate
    // [8/12/2013]
    //=========================================================================
    void ScriptComponent::Deactivate()
    {
        AZ::ScriptSystemNotificationBus::Handler::BusDisconnect();
        AZ::Data::AssetBus::Handler::BusDisconnect(m_script.GetId());

        UnloadScript();
    }

    //=========================================================================
    // ScriptComponent::OnAssetReady
    //=========================================================================
    void ScriptComponent::OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        if (asset)
        {
            m_script = asset;
            LoadScript();
        }
    }

    //=========================================================================
    // ScriptComponent::OnAssetReloaded
    //=========================================================================
    void ScriptComponent::OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset)
    {
        // When we reload a script it's tricky to maintain state for that entity instance,
        // even though theoretical mapping is possible. This feature is used for
        // faster in game iteration and based on feedbacks game designers do expect on reload to
        // reset the state.
        // If we don't need to fix the state, all we need to unload and reload the script.
        // To make sure the script behaves properly (external connections and other resources)
        // we call deactivate and activate after reload. This can cause performance issues if the
        // script registers "expensive" to register assets. If this becomes a problem we should
        // provide mapping.
        // Other things to consider are properties. Reloading will just reload the code.
        // Properties are exported from ScriptEditorComponent, so they will not be updated by
        UnloadScript();

        m_script = asset;

        LoadScript();
    }

    //=========================================================================
    // ScriptComponent::OnScriptSystemReady
    //=========================================================================
    void ScriptComponent::OnScriptSystemReady()
    {
        m_script.Create(m_sourcePath.c_str());
        AZ::Data::AssetBus::Handler::BusConnect(m_script.GetId());
        m_script.QueueLoad();
    }

    //=========================================================================
    // LoadScript
    //=========================================================================
    void ScriptComponent::LoadScript()
    {
        // Load the script, find the base table, create the entity table
        // find the Activate/Deactivate functions in the script and call them
        if (LoadInContext())
        {
            CreateEntityTable();
        }
    }

    //=========================================================================
    // UnloadScript
    //=========================================================================
    void ScriptComponent::UnloadScript()
    {
        DestroyEntityTable();
    }

    //=========================================================================
    // LoadInContext
    // [3/3/2014]
    //=========================================================================
    bool ScriptComponent::LoadInContext()
    {
        LSV_BEGIN(m_context->NativeContext(), 1);

        // Set the metamethods as we will use the script table as a metatable for entity tables
        bool success = false;
        EBUS_EVENT_RESULT(success, AZ::ScriptSystemRequestBus, Load, m_script);
        if (!success)
        {
            return false;
        }

        lua_State* lua = m_context->NativeContext();
        if (!lua_istable(lua, -1))
        {
            AZ::Data::AssetInfo assetInfo = AZ::Data::AssetManager::Instance().GetAssetInfoById(m_script.GetId());

            AZStd::string tableName = assetInfo.m_relativePath;
            AZ::StringFunc::Path::GetFileName(tableName.c_str(), tableName);
            AZStd::to_lower(tableName.begin(), tableName.end());
            m_context->Error(AZ::ScriptContext::ErrorType::Error, false, "Script %s is setup incorrectly. Scripts must now return a table (for example, you may need to add \"return %s\" to the end of the file).", assetInfo.m_relativePath.c_str(), tableName.c_str());
            lua_pop(lua, 1);
            return false;
        }

        // Point the __index of the Script table to itself
        // because it will be used as a metatable
        lua_pushliteral(lua, "__index");
        lua_pushvalue(lua, -2);
        lua_rawset(lua, -3);

        return true;
    }

    //=========================================================================
    // CreateEntityTable
    // [3/3/2014]
    //=========================================================================
    void ScriptComponent::CreateEntityTable()
    {
        lua_State* lua = m_context->NativeContext();
        LSV_BEGIN(lua, -1);

        AZ_Error("Script", lua_istable(lua, -1), "%s", "Script did not return a table!");
        int baseTableIndex = lua_gettop(lua);

        // Stack: base

        lua_createtable(lua, 0, 1); // Create entity table;
        int entityTableIndex = lua_gettop(lua);

        // Stack: base, entity

        // set my entity id
        lua_pushliteral(lua, "entityId");
        AZ::ScriptValue<AZ::EntityId>::StackPush(lua, GetEntityId());
        lua_rawset(lua, -3);

        // set the base metatable
        lua_pushvalue(lua, baseTableIndex); // copy the "Base table"
        lua_setmetatable(lua, -2); // set the scriptTable as a metatable for the entity table

        // Keep the entity table in the registry
        m_table = luaL_ref(lua, LUA_REGISTRYINDEX);

        // call OnActivate
        lua_pushliteral(lua, "OnActivate");
        lua_rawget(lua, baseTableIndex); // ScriptTable[OnActivate]
        if (lua_isfunction(lua, -1))
        {
            lua_rawgeti(lua, LUA_REGISTRYINDEX, m_table); // push the entity table as the only argument
            AZ::Internal::LuaSafeCall(lua, 1, 0); // Call OnActivate
        }
        else
        {
            lua_pop(lua, 1); // remove the OnActivate result
        }

        lua_pop(lua, 1); // remove the base property table and base script table
    }

    //=========================================================================
    // DestroyEntityTable
    //=========================================================================
    void ScriptComponent::DestroyEntityTable()
    {
        if (m_table != LUA_NOREF)
        {
            lua_State* lua = m_context->NativeContext();
            LSV_BEGIN(lua, 0);

            // call OnDeactivate
            // load table
            lua_rawgeti(lua, LUA_REGISTRYINDEX, m_table);
            if (lua_getmetatable(lua, -1) == 0) // load the base table
            {
                AZ_Assert(false, "This should not happen, all entity table should have the base table as a metatable!");
            }
            // cache
            lua_pushliteral(lua, "OnDeactivate");
            lua_rawget(lua, -2); // ScriptTable[OnDeactivte]
            if (lua_isfunction(lua, -1))
            {
                lua_pushvalue(lua, -3); // push the entity table as the only argument
                AZ::Internal::LuaSafeCall(lua, 1, 0); // Call OnDeactivate
            }
            else
            {
                lua_pop(lua, 1); // remove the OnDeactivate result
            }
            lua_pop(lua, 2); // remove the base table and the entity table

            // release table reference
            luaL_unref(lua, LUA_REGISTRYINDEX, m_table);
            m_table = LUA_NOREF;
        }
    }

    //=========================================================================
    // Reflect
    //=========================================================================
    void ScriptComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<ScriptComponent>()
                ->Field("SourceScriptName", &ScriptComponent::m_sourcePath);
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->Class<ScriptComponent>("ScriptComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_sourcePath", BehaviorValueProperty(&ScriptComponent::m_sourcePath));
        }
    }
}   // namespace AZ

#endif // #if !defined(AZCORE_EXCLUDE_LUA)

#endif  // AZ_UNITY_BUILD
