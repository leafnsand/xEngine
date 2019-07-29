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

#include <AzCore/Script/lua/lua.h>
#include <AzCore/Script/ScriptContextDebug.h>
#include <AzCore/RTTI/BehaviorContext.h>

extern "C" {
#   include <lua/lualib.h>
#   include <lua/lauxlib.h>
}

using namespace AZ;

//=========================================================================
// ScriptContextDebug
// [6/29/2012]
//=========================================================================
ScriptContextDebug::ScriptContextDebug(ScriptContext& scriptContext)
    : m_context(scriptContext)
{
}

//=========================================================================
// ~ScriptContextDebug
// [6/29/2012]
//=========================================================================
ScriptContextDebug::~ScriptContextDebug()
{
}

//=========================================================================
// EnumRegisteredClasses
// [4/5/2012]
//=========================================================================
void
ScriptContextDebug::EnumRegisteredClasses(EnumClass enumClass, EnumMethod enumMethod, EnumProperty enumProperty, void* userData)
{
    AZ_Assert(enumClass && enumMethod && enumProperty, "Invalid input!");

    lua_State* l = m_context.NativeContext();
    lua_rawgeti(l, LUA_REGISTRYINDEX, AZ_LUA_CLASS_TABLE_REF); // load the class table

    lua_pushnil(l);

    while (lua_next(l, -2) != 0)
    {
        if (!lua_isuserdata(l, -2))
        {
            lua_pop(l, 1);
            continue;
        }

        if (lua_istable(l, -1))
        {
            lua_rawgeti(l, -1, AZ_LUA_CLASS_METATABLE_BEHAVIOR_CLASS);
            BehaviorClass* behaviorClass = reinterpret_cast<BehaviorClass*>(lua_touserdata(l, -1));

            lua_rawgeti(l, -2, AZ_LUA_CLASS_METATABLE_NAME_INDEX); // load class name
            AZ_Assert(lua_isstring(l, -1), "Internal scipt error: class without a classname at index %d", AZ_LUA_CLASS_METATABLE_NAME_INDEX);
            
            if (!enumClass(lua_tostring(l, -1), behaviorClass->m_typeId, userData))
            {
                lua_pop(l, 5);
                return;
            }
            lua_pop(l, 2); // pop the Class name and behaviorClass

            lua_pushnil(l);
            while (lua_next(l, -2) != 0)
            {
                if (lua_isstring(l, -2) && lua_isfunction(l, -1))
                {
                    const char* name = lua_tostring(l, -2);
                    if (lua_tocfunction(l, -1) == &Internal::LuaPropertyTagHelper) // if it's a property
                    {
                        bool isRead = true;
                        bool isWrite = true;

                        lua_getupvalue(l, -1, 1);
                        if (lua_isnil(l, -1))
                        {
                            isRead = false;
                        }
                        lua_pop(l, 1);

                        lua_getupvalue(l, -1, 2);
                        if (lua_isnil(l, -1))
                        {
                            isWrite = false;
                        }
                        lua_pop(l, 1);

                        if (!enumProperty(&behaviorClass->m_typeId, name, isRead, isWrite, userData))
                        {
                            lua_pop(l, 5);
                            return;
                        }
                    }
                    else
                    {
                        if (strncmp(name, "__", 2) != 0)
                        {
                            const char* dbgParamInfo = NULL;
                            lua_getupvalue(l, -1, 2);
                            if (lua_isstring(l, -1))
                            {
                                dbgParamInfo = lua_tostring(l, -1);
                            }

                            if (!enumMethod(&behaviorClass->m_typeId, name, dbgParamInfo, userData))
                            {
                                lua_pop(l, 6);
                                return;
                            }
                            lua_pop(l, 1); // pop the DBG name
                        }
                    }
                }
                lua_pop(l, 1);
            }
        }
        lua_pop(l, 1);
    }
    lua_pop(l, 1); // pop the class table
}

//=========================================================================
// EnumRegisteredGlobals
// [4/5/2012]
//=========================================================================
void
ScriptContextDebug::EnumRegisteredGlobals(EnumMethod enumMethod, EnumProperty enumProperty, void* userData)
{
    lua_State* l = m_context.NativeContext();
    lua_rawgeti(l, LUA_REGISTRYINDEX, AZ_LUA_GLOBALS_TABLE_REF); // load the class table

    //AZ_TracePrintf("Script","\nGlobals\n");
    lua_pushnil(l);
    while (lua_next(l, -2) != 0)
    {
        if (lua_isstring(l, -2) && lua_isfunction(l, -1))
        {
            const char* name = lua_tostring(l, -2);
            if (lua_tocfunction(l, -1) == &Internal::LuaPropertyTagHelper) // if it's a property
            {
                bool isRead = true;
                bool isWrite = true;

                lua_getupvalue(l, -1, 1);
                if (lua_isnil(l, -1))
                {
                    isRead = false;
                }
                lua_pop(l, 1);

                lua_getupvalue(l, -1, 2);
                if (lua_isnil(l, -1))
                {
                    isWrite = false;
                }
                lua_pop(l, 1);

                //AZ_TracePrintf("Script"," -- property %s %s\n",name,propType);
                if (!enumProperty(nullptr, name, isRead, isWrite, userData))
                {
                    lua_pop(l, 3);
                    return;
                }
            }
            else
            {
                if (strncmp(name, "__", 2) != 0)
                {
                    const char* dbgParamInfo = NULL;
                    lua_getupvalue(l, -1, 2);
                    if (lua_isstring(l, -1))
                    {
                        dbgParamInfo = lua_tostring(l, -1);
                    }

                    if (!enumMethod(nullptr, name, dbgParamInfo, userData))
                    {
                        lua_pop(l, 4);
                        return;
                    }
                    lua_pop(l, 1); // pop the dbgParamInfo
                }
            }
        }
        lua_pop(l, 1);
    }

    lua_pop(l, 1); // pop globals table
}

// TODO: We need to move this to the string funcs file or use functions existing there
size_t StringReplaceAll(AZStd::string& toReplace, const AZStd::string& search, const AZStd::string& replace)
{
    size_t pos = 0;
    size_t numReplaced = 0;
    while (true)
    {
        pos = toReplace.find(search, pos);
        if (pos == AZStd::string::npos)
        {
            break;
        }
        if (search.length() == replace.length())
        {
            toReplace.replace(pos, search.length(), replace);
        }
        else
        {
            toReplace.erase(pos, search.length());
            toReplace.insert(pos, replace);
        }
        pos += replace.length();
        ++numReplaced;
    }

    return numReplaced;
}

// TODO: Move theis to the string funcs file and possibly retool it to use existing functions there
void StripQualifiers(AZStd::string& token)
{
    StringReplaceAll(token, "&", "");
    StringReplaceAll(token, "*", "");
    StringReplaceAll(token, "const ", "");
}

bool EBusCanBroadcast(const AZ::BehaviorEBus* bus)
{
    auto canBroadcast = [](const AZStd::pair<AZStd::string, AZ::BehaviorEBusEventSender>& pair) { return pair.second.m_broadcast != nullptr; };
    return !FindAttribute(Script::Attributes::DisallowBroadcast, bus->m_attributes) && AZStd::find_if(bus->m_events.begin(), bus->m_events.end(), canBroadcast) != bus->m_events.end();
}

bool EBusCanQueue(const AZ::BehaviorEBus* bus)
{
    auto canQueue = [](const AZStd::pair<AZStd::string, AZ::BehaviorEBusEventSender>& pair) { return pair.second.m_queueEvent != nullptr || pair.second.m_queueBroadcast != nullptr; };
    return AZStd::find_if(bus->m_events.begin(), bus->m_events.end(), canQueue) != bus->m_events.end();
}

bool EBusHasHandler(const AZ::BehaviorEBus* bus)
{
    return bus->m_createHandler && bus->m_destroyHandler;
}

void ScriptContextDebug::EnumRegisteredEBuses(EnumEBus enumEBus, EnumEBusSender enumEBusSender, void* userData)
{
    AZ::BehaviorContext* behaviorContext = m_context.GetBoundContext();
    if (!behaviorContext)
    {
        return;
    }

    for (auto const &ebusPair : behaviorContext->m_ebuses)
    {
        AZ::BehaviorEBus* ebus = ebusPair.second;
        bool canBroadcast = EBusCanBroadcast(ebus);
        bool canQueue = EBusCanQueue(ebus);
        bool hasHandler = EBusHasHandler(ebus);

        enumEBus(ebus->m_name, canBroadcast, canQueue, hasHandler, userData);

        for (auto const eventSender : ebus->m_events)
        {
            if (eventSender.second.m_event)
            {
                // Find a func that does this already!
                AZStd::string scriptArgs;
                for (size_t i = 0; i < eventSender.second.m_event->GetNumArguments(); ++i)
                {
                    AZStd::string argName = eventSender.second.m_event->GetArgument(i)->m_name;
                    StripQualifiers(argName);
                    scriptArgs += argName;
                    if (i != eventSender.second.m_event->GetNumArguments() - 1)
                    {
                        scriptArgs += ", ";
                    }
                }

                AZStd::string funcName = eventSender.first;
                StripQualifiers(funcName);

                enumEBusSender(ebus->m_name, funcName, scriptArgs, "Event", userData);
            }
            if (eventSender.second.m_broadcast && canBroadcast)
            {
                // Find a func that does this already!
                AZStd::string scriptArgs;
                for (size_t i = 0; i < eventSender.second.m_broadcast->GetNumArguments(); ++i)
                {
                    AZStd::string argName = eventSender.second.m_broadcast->GetArgument(i)->m_name;
                    StripQualifiers(argName);
                    scriptArgs += argName;
                    if (i != eventSender.second.m_broadcast->GetNumArguments() - 1)
                    {
                        scriptArgs += ", ";
                    }
                }

                AZStd::string funcName = eventSender.first;
                StripQualifiers(funcName);

                enumEBusSender(ebus->m_name, funcName, scriptArgs, "Broadcast", userData);
            }
        }

        if (ebus->m_createHandler)
        {
            AZ::BehaviorEBusHandler* handler = nullptr;
            ebus->m_createHandler->InvokeResult(handler);
            const auto& notifications = handler->GetEvents();
            for (const auto& notification : notifications)
            {
                AZStd::string scriptArgs;
                const size_t paramCount = notification.m_parameters.size();
                for (size_t i = 0; i < notification.m_parameters.size(); ++i)
                {
                    AZStd::string argName = notification.m_parameters[i].m_name;
                    StripQualifiers(argName);
                    scriptArgs += argName;
                    if (i != paramCount - 1)
                    {
                        scriptArgs += ", ";
                    }
                }

                AZStd::string funcName = notification.m_name;
                StripQualifiers(funcName);

                enumEBusSender(ebus->m_name, funcName, scriptArgs, "Notification", userData);
            }
            ebus->m_destroyHandler->Invoke(handler);
        }
    }
}

#endif // #if !defined(AZCORE_EXCLUDE_LUA)

#endif // #ifndef AZ_UNITY_BUILD