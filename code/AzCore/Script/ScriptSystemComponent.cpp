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

#include <AzCore/Script/ScriptSystemComponent.h>

#include <AzCore/Asset/AssetManagerBus.h>
#include <AzCore/Asset/AssetManager.h>
#include <AzCore/Casting/numeric_cast.h>
#include <AzCore/Component/ComponentApplication.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/Math/MathReflection.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Script/ScriptAsset.h>
#include <AzCore/Script/ScriptDebug.h>
#include <AzCore/Script/ScriptContextDebug.h>
#include <AzCore/StringFunc/StringFunc.h>
#include <AzCore/std/string/conversions.h>
#include <AzCore/Script/lua/lua.h>
#include <AzCore/JSON/rapidjson.h>
#include <AzCore/FileFunc/FileFunc.h>

using namespace AZ;

/**
 * Script lifecycle:
 * 1. When a script is requested for load, LoadAssetData() is called by the asset database
 * 2. LoadAssetData will load the script into a buffer.
 * 3. Load will compile the script, execute the function, and return with the resulting table on top of the stack.
 *      This table will also be cached, so subsequent loads of the same script/vm will not recompile.
 * 4. On change, all references to the script will be removed.
 *      If the script has been required, the script will no longer be tracked.
 *      If the script was loaded by a ScriptComponent, Load will be called once reload is complete.
 */

namespace
{
    // Called when a module has already been loaded
    static int LuaRequireLoadedModule(lua_State* l)
    {
        // Push value to top of stack
        lua_pushvalue(l, lua_upvalueindex(1));

        return 1;
    }
}

//=========================================================================
// ScriptSystemComponent
// [5/29/2012]
//=========================================================================
ScriptSystemComponent::ScriptSystemComponent()
{
    m_defaultGarbageCollectorSteps = 2; // this is a default value, users should tweak this number for optimal performance
}

//=========================================================================
// ~ScriptSystemComponent
// [5/29/2012]
//=========================================================================
ScriptSystemComponent::~ScriptSystemComponent()
{
}

//=========================================================================
// Activate
// [5/29/2012]
//=========================================================================
void ScriptSystemComponent::Activate()
{
    // Create default context
    m_contextContainer = aznew ContextContainer;
    m_contextContainer->m_context = aznew ScriptContext();
    m_contextContainer->m_garbageCollectorSteps = m_defaultGarbageCollectorSteps;

    m_contextContainer->m_context->SetRequireHook(AZStd::bind(&ScriptSystemComponent::DefaultRequireHook, this, AZStd::placeholders::_1, AZStd::placeholders::_2, AZStd::placeholders::_3));

    // Reflect script classes
    ComponentApplication* app = nullptr;
    EBUS_EVENT_RESULT(app, ComponentApplicationBus, GetApplication);
    if (app && app->GetDescriptor().m_enableScriptReflection)
    {
        if (app->GetBehaviorContext())
        {
            m_contextContainer->m_context->BindTo(app->GetBehaviorContext());
        }
        else
        {
            AZ_Error("Script", false, "We are asked to enabled scripting, but the Applicaion has no BehaviorContext! Scripting relies on BehaviorContext!");
        }
    }

    ScriptSystemRequestBus::Handler::BusConnect();

    SystemTickBus::Handler::BusConnect();

    if (Data::AssetManager::Instance().IsReady())
    {
        Data::AssetManager::Instance().RegisterHandler(this, AzTypeInfo<ScriptAsset>::Uuid());
    }

    m_fileList.Create("lua/filelist");
    Data::AssetBus::MultiHandler::BusConnect(m_fileList.GetId());
    m_fileList.QueueLoad();
}

//=========================================================================
// Deactivate
// [5/29/2012]
//=========================================================================
void ScriptSystemComponent::Deactivate()
{
    m_fileList.Release();
    m_scriptAssets.clear();

    SystemTickBus::Handler::BusDisconnect();
    ScriptSystemRequestBus::Handler::BusDisconnect();
    Data::AssetBus::MultiHandler::BusDisconnect();

    {
        // Lock access to the loaded scripts map
        AZStd::lock_guard<AZStd::recursive_mutex> lock(m_contextContainer->m_loadedScriptsMutex);

        // Clear loaded scripts, they need the context to still be around on unload
        m_contextContainer->m_loadedScripts.clear();
        delete m_contextContainer->m_context;
    }

    delete m_contextContainer;

    // Need to do this at the end, so that any cached scripts cleared above may be released properly
    if (Data::AssetManager::Instance().IsReady())
    {
        Data::AssetManager::Instance().UnregisterHandler(this);
    }
}

//=========================================================================
// GetContext
// [3/1/2014]
//=========================================================================
ScriptContext* ScriptSystemComponent::GetContext()
{
    return m_contextContainer->m_context;
}

//=========================================================================
// OnSystemTick
//=========================================================================
void ScriptSystemComponent::OnSystemTick()
{
    m_contextContainer->m_context->GarbageCollectStep(m_contextContainer->m_garbageCollectorSteps);
}

//=========================================================================
// OnAssetReady
//=========================================================================
void ScriptSystemComponent::OnAssetReady(Data::Asset<Data::AssetData> asset)
{
    Data::AssetBus::MultiHandler::BusDisconnect(asset.GetId());

    if (asset == m_fileList)
    {
        AZStd::string content((const char*)m_fileList.Get()->GetBuffer(), m_fileList.Get()->GetLength());
        AZStd::vector<AZStd::string> lines;
        AZ::StringFunc::Tokenize(content.c_str(), lines, "\r\n");
        for (auto& line : lines)
        {
            AZStd::string filePath = "lua/" + line;
            auto loadAsset = Data::AssetManager::Instance().GetAsset<ScriptAsset>(filePath.c_str());
            Data::AssetBus::MultiHandler::BusConnect(loadAsset.GetId());
            m_scriptAssets.push_back(loadAsset);
        }
    }
    else
    {
        if (IsReady())
        {
            EBUS_EVENT(ScriptSystemNotificationBus, OnScriptSystemReady);
        }
    }
}

//=========================================================================
// OnAssetError
//=========================================================================
void ScriptSystemComponent::OnAssetError(Data::Asset<Data::AssetData> asset)
{
    Data::AssetBus::MultiHandler::BusDisconnect(asset.GetId());

    if (asset == m_fileList)
    {
        AZ_Error("Script", false, "lua filelist do not exist!\n");
    }
    else
    {
        AZ_Error("Script", false, "lua file %s do not exist!\n", asset.GetHint().c_str());
        if (IsReady())
        {
            EBUS_EVENT(ScriptSystemNotificationBus, OnScriptSystemReady);
        }
    }
}

//=========================================================================
// GarbageCollect
//=========================================================================
void ScriptSystemComponent::GarbageCollect()
{
    m_contextContainer->m_context->GarbageCollect();
}

//=========================================================================
// GarbageCollectStep
//=========================================================================
void ScriptSystemComponent::GarbageCollectStep(int numberOfSteps)
{
    m_contextContainer->m_context->GarbageCollectStep(numberOfSteps);
}

static bool EnumClass(const char* className, const AZ::Uuid& classTypeId, void* userdata)
{
    rapidjson::Value* document = static_cast<rapidjson::Value*>(userdata);
    if (!document)
    {
        return false;
    }
    AZ_TracePrintf("", "class: %s\n", className);
    return true;
}

static bool EnumMethod(const AZ::Uuid* classTypeId, const char* methodName, const char* debugArgumentInfo, void* userdata)
{
    AZ_TracePrintf("", "method: %s, %s\n", methodName, debugArgumentInfo);
    return true;
}

static bool EnumProperty(const AZ::Uuid* classTypeId, const char* propertyName, bool canRead, bool canWrite, void* userdata)
{
    AZ_TracePrintf("", "property: %s, read: %s, write: %s\n", propertyName, canRead ? "true" : "false", canWrite ? "true" : "false");
    return true;
}

static bool EnumEBus(const AZStd::string& ebusName, bool canBroadcast, bool canQueue, bool hasHandler, void* userdata)
{
    AZ_TracePrintf("", "ebus: %s, broadcast: %s, queue: %s, notification: %s\n", ebusName.c_str(), canBroadcast ? "true" : "false", canQueue ? "true" : "false", hasHandler ? "true" : "false");
    return true;
}

static bool EnumEBusSender(const AZStd::string& ebusName, const AZStd::string& senderName, const AZStd::string& debugArgumentInfo, const AZStd::string& category, void* userdata)
{
    AZ_TracePrintf("", "ebus sender: %s, %s, %s, %s\n", ebusName.c_str(), senderName.c_str(), debugArgumentInfo.c_str(), category.c_str());
    return true;
}

//=========================================================================
// SaveAllReflections
//=========================================================================
void ScriptSystemComponent::WriteAllReflections(const char* path)
{
    if (!m_contextContainer->m_context->GetDebugContext())
    {
        m_contextContainer->m_context->EnableDebug();
    }

    // rapidjson::Document document;
    // document.SetObject();

    m_contextContainer->m_context->GetDebugContext()->EnumRegisteredGlobals(EnumMethod, EnumProperty, nullptr);
    m_contextContainer->m_context->GetDebugContext()->EnumRegisteredClasses(EnumClass, EnumMethod, EnumProperty, nullptr);
    m_contextContainer->m_context->GetDebugContext()->EnumRegisteredEBuses(EnumEBus, EnumEBusSender, nullptr);

    // if (!FileFunc::WriteJsonFile(document, path))
    // {
    //     AZ_Warning("ComponentApplication", false, "Failed to save json to %s.", path);
    // }
}

bool ScriptSystemComponent::Load(const Data::Asset<ScriptAsset>& asset)
{
    ScriptContext* context = m_contextContainer->m_context;
    lua_State* lua = context->NativeContext();

    {
        // Lock access to the loaded scripts map
        AZStd::lock_guard<AZStd::recursive_mutex> lock(m_contextContainer->m_loadedScriptsMutex);

        // Check if already loaded
        auto scriptIt = m_contextContainer->m_loadedScripts.find(asset.GetId().m_guid);
        if (scriptIt != m_contextContainer->m_loadedScripts.end())
        {
            lua_rawgeti(lua, LUA_REGISTRYINDEX, scriptIt->second.m_tableReference);

            // If not table, pop and return false
            if (!lua_istable(lua, -1))
            {
                lua_pop(lua, 1);
                return false;
            }
            else
            {
                return true;
            }
        }
    }

    // Load lua script into the VM
    IO::MemoryStream stream = asset.Get()->CreateMemoryStream();
    if (!context->LoadFromStream(&stream, asset.Get()->GetDebugName(), lua))
    {
        context->Error(ScriptContext::ErrorType::Error, "%s", lua_tostring(lua, -1));
        lua_pop(lua, 1);
        return false;
    }

    // Execute the script
    if (!Internal::LuaSafeCall(lua, 0, 1))
    {
        return false;
    }

    // Dupe the table (so we can ref AND return it)
    lua_pushvalue(lua, -1);
    int ref = luaL_ref(lua, LUA_REGISTRYINDEX);

    // No need to keep the script asset around, the owner will keep a reference to it (or this, if it's require()'d)
    LoadedScriptInfo info;
    const char* debugName = asset.Get()->GetDebugName();
    if (debugName)
    {
        info.m_scriptNames.emplace(debugName);
    }
    info.m_tableReference = ref;

    {
        // Lock access to the loaded scripts map
        AZStd::lock_guard<AZStd::recursive_mutex> lock(m_contextContainer->m_loadedScriptsMutex);
        m_contextContainer->m_loadedScripts.emplace(asset.GetId().m_guid, AZStd::move(info));
    }

    return true;
}

bool ScriptSystemComponent::IsReady()
{
    return m_fileList.IsReady() && AZStd::find_if(m_scriptAssets.begin(), m_scriptAssets.end(), [](const Data::Asset<ScriptAsset>& it)
    {
        return it.IsLoading();
    }
    ) == m_scriptAssets.end();
}

/**
 * Hook called when requiring a script
 *
 * Step 1: Build file path from module name requested
 * Step 2: Lookup file path in asset database
 * If already loaded:
 *  Step 3: Push cached result of asset to top of stack
 *  Step 4: Push function (above) that simply returns that value (require() expects either a function (success) or string (error message) pushed to the stack)
 * Else:
 *  Step 3: Request load of asset
 *  Step 4: Short circuit asset loading, and load asset blocking (require waits for no man)
 * Step 5: Register the script with the script system component, so that on asset unload, we may cache it (see TrackRequiredScript for more details)
 */
int ScriptSystemComponent::DefaultRequireHook(lua_State* lua, ScriptContext* context, const char* module)
{
    // File extension for lua files
    static const char* s_luaExtension = ".lua";

    // Replace '.' in module name with '/'
    AZStd::string filePath = "lua.";
    filePath += module;
    for (auto pos = filePath.find('.'); pos != AZStd::string::npos; pos = filePath.find('.'))
    {
        filePath.replace(pos, 1, "/", 1);
    }

    // Add file extension to path
    if (filePath.find(s_luaExtension) == AZStd::string::npos)
    {
        filePath += s_luaExtension;
    }

    Data::AssetId scriptId = Data::AssetManager::Instance().GetAssetIdByPath(filePath.c_str(), azrtti_typeid<ScriptAsset>());

    // Lock access to the loaded scripts map
    AZStd::lock_guard<AZStd::recursive_mutex> lock(m_contextContainer->m_loadedScriptsMutex);

    // If script already loaded in this context, just use its resulting table
    auto scriptIt = m_contextContainer->m_loadedScripts.find(scriptId.m_guid);
    if (scriptIt != m_contextContainer->m_loadedScripts.end())
    {
        // Add the name used for require as an alias
        scriptIt->second.m_scriptNames.emplace(module);
        // Push the value to a closure that will just return it
        lua_rawgeti(lua, LUA_REGISTRYINDEX, scriptIt->second.m_tableReference);
        lua_pushcclosure(lua, LuaRequireLoadedModule, 1);

        // If asset reference already populated, just return now. Otherwise, capture reference
        if (scriptIt->second.m_scriptAsset.GetId().IsValid())
        {
            return 1;
        }
    }

    // Grab a reference to the script asset being loaded
    Data::Asset<ScriptAsset> script = Data::AssetManager::Instance().FindAsset<ScriptAsset>(scriptId);

    // If it's not loaded, load it in a blocking manner
    if (!script.IsReady())
    {
        script = Data::AssetManager::Instance().GetAsset<ScriptAsset>(scriptId, true, nullptr, true);
        if (!script.IsReady())
        {
            lua_pushfstring(lua, "Module \"%s\" failed to load.", filePath.c_str());
            return 1;
        }
    }

    // Now that we have a valid asset reference, stash it and return if the script already loaded and tracked
    if (scriptIt != m_contextContainer->m_loadedScripts.end())
    {
        scriptIt->second.m_scriptAsset = script;
        return 1;
    }

    // If not, load the script, getting the resulting table
    if (!Load(script))
    {
        // If the load fails, return the error string load pushed onto the stack
        return 1;
    }

    // Push function returning the result
    lua_pushcclosure(lua, LuaRequireLoadedModule, 1);

    // Set asset reference on the loaded script
    scriptIt = m_contextContainer->m_loadedScripts.find(scriptId.m_guid);
    scriptIt->second.m_scriptNames.emplace(module);
    scriptIt->second.m_scriptAsset = script;

    return 1;
}

void ScriptSystemComponent::ClearAssetReferences(Data::AssetId assetBaseId)
{
    {
        AZStd::lock_guard<AZStd::recursive_mutex> lock(m_contextContainer->m_loadedScriptsMutex);

        auto scriptIt = m_contextContainer->m_loadedScripts.find(assetBaseId.m_guid);
        if (scriptIt != m_contextContainer->m_loadedScripts.end())
        {
            lua_State* l = m_contextContainer->m_context->NativeContext();

            // Spin off thread for clearing the loaded table
            LuaNativeThread thread(l);

            lua_getfield(thread, LUA_REGISTRYINDEX, "_LOADED");
            for (const auto& modName : scriptIt->second.m_scriptNames)
            {
                lua_pushnil(thread);
                lua_setfield(thread, -2, modName.c_str());
            }

            // Unref the script table so it may be collected
            luaL_unref(thread, LUA_REGISTRYINDEX, scriptIt->second.m_tableReference);

            // Now that it has been removed, don't track it until it's required again (replacing it in this list).
            m_contextContainer->m_loadedScripts.erase(scriptIt);
        }
    }
}

//=========================================================================
// CreateAsset
// [3/6/2014]
//=========================================================================
Data::AssetPtr ScriptSystemComponent::CreateAsset(const Data::AssetId& id, const Data::AssetType&)
{
    return aznew ScriptAsset(id);
}

//=========================================================================
// LoadAssetData
//
// Loads the script into the Lua VM. The result of this function is
// asset->GetRegistryIndex() being a reference to a function.
//=========================================================================
bool ScriptSystemComponent::LoadAssetData(const Data::Asset<Data::AssetData>& asset, IO::GenericStream* stream, const Data::AssetFilterCB& assetLoadFilterCB)
{
    (void)assetLoadFilterCB;

    if (stream)
    {
        ScriptAsset* script = asset.GetAs<ScriptAsset>();

        Data::AssetInfo scriptInfo = Data::AssetManager::Instance().GetAssetInfoById(asset.GetId());
        script->m_debugName = "@" + scriptInfo.m_relativePath;
        AZStd::to_lower(script->m_debugName.begin(), script->m_debugName.end());

        // Read contents into buffer
        size_t scriptDataLength = stream->GetLength() - stream->GetCurPos(); // Remove already read characters from length

        AZ_Error("Script", scriptDataLength > 0, "Script contents are empty! Please check AssetProcessor output.");

        script->m_scriptBuffer.resize(scriptDataLength);
        stream->Read(scriptDataLength, script->m_scriptBuffer.data());

        // Clear cached references in the event of a successful load
        TickBus::QueueFunction(&ScriptSystemComponent::ClearAssetReferences, this, asset.GetId());

        return true;
    }

    return false;
}

//=========================================================================
// DestroyAsset
//
// Unref the table returned by the script, so that:
//  * It may be garbage collected
//  * This asset isn't used again in the future
//=========================================================================
void ScriptSystemComponent::DestroyAsset(Data::AssetPtr ptr)
{
    delete ptr;
}

//=========================================================================
// GetProvidedServices
//=========================================================================
void ScriptSystemComponent::GetProvidedServices(ComponentDescriptor::DependencyArrayType& provided)
{
    provided.push_back(AZ_CRC("ScriptService", 0x787235ab));
}

//=========================================================================
// GetIncompatibleServices
//=========================================================================
void ScriptSystemComponent::GetIncompatibleServices(ComponentDescriptor::DependencyArrayType& incompatible)
{
    incompatible.push_back(AZ_CRC("ScriptService", 0x787235ab));
}

//=========================================================================
// GetDependentServices
//=========================================================================
void ScriptSystemComponent::GetDependentServices(ComponentDescriptor::DependencyArrayType& dependent)
{
    dependent.push_back(AZ_CRC("AssetDatabaseService", 0x3abf5601));
}

/**
 * Behavior Context forwarder
 */
class TickBusBehaviorHandler : public TickBus::Handler, public AZ::BehaviorEBusHandler
{
public:
    AZ_EBUS_BEHAVIOR_BINDER(TickBusBehaviorHandler, "{EE90D2DA-9339-4CE6-AF98-AF81E00E2AB3}", AZ::SystemAllocator, OnTick, GetTickOrder);

    void OnTick(float deltaTime, ScriptTimePoint time) override
    {
        Call(FN_OnTick,deltaTime,time);
    }

    int GetTickOrder() override
    {
        int order = ComponentTickBus::TICK_DEFAULT;
        CallResult(order, FN_GetTickOrder);
        return order;
    }
};

// Dummy class to host ComponentTickBus enums
struct DummyTickOrder
{
    AZ_TYPE_INFO(DummyTickOrder, "{8F725746-A4BC-4DF7-A249-02931973C864}");
};

//=========================================================================
// Reflect
//=========================================================================
void ScriptSystemComponent::Reflect(ReflectContext* reflection)
{
    ScriptTimePoint::Reflect(reflection);

    if (SerializeContext* serializeContext = azrtti_cast<SerializeContext*>(reflection))
    {
        
        serializeContext->Class<ScriptSystemComponent, AZ::Component>()->
            Field("garbageCollectorSteps", &ScriptSystemComponent::m_defaultGarbageCollectorSteps);
    }

    if (BehaviorContext* behaviorContext = azrtti_cast<BehaviorContext*>(reflection))
    {
        // reflect default entity
        MathReflect(behaviorContext);
        ScriptDebug::Reflect(behaviorContext);

        behaviorContext->Class<PlatformID>("Platform")
            ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
            ->Enum<PLATFORM_WINDOWS_32>("Windows32")
            ->Enum<PLATFORM_WINDOWS_64>("Windows64") 
            ->Enum<PLATFORM_XBOX_360>("Xbox360") // ACCEPTED_USE
            ->Enum<PLATFORM_XBONE>("XboxOne") // ACCEPTED_USE
            ->Enum<PLATFORM_PS3>("PS3") // ACCEPTED_USE
            ->Enum<PLATFORM_PS4>("PS4") // ACCEPTED_USE
            ->Enum<PLATFORM_WII>("Wii") // ACCEPTED_USE
            ->Enum<PLATFORM_LINUX_64>("Linux")
            ->Enum<PLATFORM_ANDROID>("Android")
            ->Enum<PLATFORM_ANDROID_64>("Android64")
            ->Enum<PLATFORM_APPLE_IOS>("iOS")
            ->Enum<PLATFORM_APPLE_OSX>("OSX")
            ->Enum<PLATFORM_APPLE_TV>("tvOS")
            ->Property("Current", BehaviorConstant(AZ::g_currentPlatform), nullptr)
            ->Method("GetName", &AZ::GetPlatformName)
            ;

        behaviorContext->EBus<TickBus>("TickBus")
            ->Handler<TickBusBehaviorHandler>()
            ;

        behaviorContext->EBus<TickRequestBus>("TickRequestBus")
            ->Event("GetTickDeltaTime", &TickRequestBus::Events::GetTickDeltaTime)
            ->Event("GetTimeAtCurrentTick", &TickRequestBus::Events::GetTimeAtCurrentTick)
            ;

        behaviorContext->Class<DummyTickOrder>("TickOrder")
            ->Enum<TICK_FIRST>("First")
            ->Enum<TICK_PLACEMENT>("Placement")
            ->Enum<TICK_ANIMATION>("Animation")
            ->Enum<TICK_PHYSICS>("Physics")
            ->Enum<TICK_DEFAULT>("Default")
            ->Enum<TICK_UI>("UI")
            ->Enum<TICK_LAST>("Last")
            ;
    }
}
#endif // #if !defined(AZCORE_EXCLUDE_LUA)

#endif // #ifndef AZ_UNITY_BUILD
