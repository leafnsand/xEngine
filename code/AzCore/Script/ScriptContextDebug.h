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
#ifndef AZCORE_SCRIPT_CONTEXT_DEBUG_H
#define AZCORE_SCRIPT_CONTEXT_DEBUG_H

#include <AzCore/Script/ScriptContext.h>
#include <AzCore/std/functional.h>
#include <AzCore/std/containers/unordered_map.h>
#include <AzCore/std/string/string.h>
#include <AzCore/std/string/osstring.h>
#include <AzCore/Memory/OSAllocator.h>

namespace AZ
{
    /**
     * Debug functionality for a script context (VM).
     * This context will be generated automatically once you enable the Debugging in the \ref ScriptContext.
     * Use this class to obtain registered classes, better callstacks, breakpoints, etc.
     */
    class ScriptContextDebug
    {
        ScriptContextDebug(const ScriptContextDebug&);
        ScriptContextDebug operator=(const ScriptContextDebug&);

    public:
        AZ_CLASS_ALLOCATOR(ScriptContextDebug, SystemAllocator, 0)

        ScriptContextDebug(ScriptContext& scriptContext);
        ~ScriptContextDebug();

        // print functions return true to continue and false to stop enum.
        typedef bool(*EnumClass)(const char* /* class Name*/, const AZ::Uuid& /* class TypeId */, void* /* userdata */);
        typedef bool(*EnumMethod)(const AZ::Uuid* /* class TypeId or null if global*/, const char* /* method name */, const char* /* debug argument info (can be null) */, void* /* user data */);
        typedef bool(*EnumProperty)(const AZ::Uuid* /* class TypeId or null if global*/, const char* /* property Name*/, bool /* can read property */, bool /* can write property */, void* /* user data */);
        typedef bool(*EnumEBus)(const AZStd::string& /*ebus name*/, bool /* canBroadcast */, bool /* canQueue */, bool /* hasHandler  */, void* /* userdata */);
        typedef bool(*EnumEBusSender)(const AZStd::string& /*ebus*/, const AZStd::string& /*Sender Name*/, const AZStd::string& /*debug argument info*/, const AZStd::string& category, void* /*user data*/);

        // Enum registered classes, this is enabled even in final builds
        void                EnumRegisteredClasses(EnumClass enumClass, EnumMethod enumMethod, EnumProperty enumProperty, void* userData = NULL);
        // Enum registered global properties and methods, this is enabled even in final builds
        void                EnumRegisteredGlobals(EnumMethod enumMethod, EnumProperty enumProperty, void* userData = NULL);
        // Enum registered ebusses including senders and handlers, this is enabled even in final builds
        void                EnumRegisteredEBuses(EnumEBus enumEBus, EnumEBusSender enumEBusSender, void* userData = NULL);

    private:
        ScriptContext&       m_context;
    };
}

#endif // AZCORE_SCRIPT_CONTEXT_DEBUG_H
#pragma once