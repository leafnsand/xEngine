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
#ifndef AZCORE_SCRIPT_SYSTEM_BUS_H
#define AZCORE_SCRIPT_SYSTEM_BUS_H

#include <AzCore/EBus/EBus.h>
#include <AzCore/Script/ScriptAsset.h>
#include <AzCore/Script/ScriptContext.h>

namespace AZ
{
    /**
     * Event for communication with the component main application. There can
     * be only one application at a time. This is why this but is set to support
     * only one client/listener.
     */
    class ScriptSystemRequests
        : public AZ::EBusTraits
    {
    public:
        virtual ~ScriptSystemRequests() {}

        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides - application is a singleton
        static const AZ::EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Single;  // we sort components on m_initOrder
        //////////////////////////////////////////////////////////////////////////

        /// Returns the script context that has been registered with the app, if there is one.
        virtual ScriptContext* GetContext() = 0;

        /// Full GC will be performed
        virtual void GarbageCollect() = 0;

        /// Step GC 
        virtual void GarbageCollectStep(int numberOfSteps) = 0;

        /// save lua api
        virtual void WriteAllReflections(const char* path) = 0;

        /**
         * Load script asset into the a context.
         * If the load succeeds, the script table will be on top of the stack
         *
         * \param asset     script asset
         * \param id        the id of the context to load the script in
         *
         * \return whether or not the asset load succeeded
         */
        virtual bool Load(const Data::Asset<ScriptAsset>& asset) = 0;

        /// all lua file in filelist is loaded
        virtual bool IsReady() = 0;
    };

    using ScriptSystemRequestBus = AZ::EBus<ScriptSystemRequests>;

    class ScriptSystemNotification : public AZ::EBusTraits
    {
    public:
        virtual ~ScriptSystemNotification() = default;

        static const AZ::EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Multiple;

        static const AZ::EBusAddressPolicy AddressPolicy = EBusAddressPolicy::Single;

        virtual void OnScriptSystemReady() {}

        template<class Bus>
        struct ConnectionPolicy
            : public AZ::EBusConnectionPolicy<Bus>
        {
            static void Connect(typename Bus::BusPtr& busPtr, typename Bus::Context& context, typename Bus::HandlerNode& handler, const typename Bus::BusIdType& id = 0)
            {
                AZ::EBusConnectionPolicy<Bus>::Connect(busPtr, context, handler, id);

                bool isReady = false;
                EBUS_EVENT_RESULT(isReady, ScriptSystemRequestBus, IsReady);
                typename Bus::template CallstackEntryIterator<typename Bus::InterfaceType**> callstack(nullptr, &id); // Workaround for GetCurrentBusId in callee
                if (isReady)
                {
                    handler->OnScriptSystemReady();
                }
            }
        };
    };

    using ScriptSystemNotificationBus = AZ::EBus<ScriptSystemNotification>;
}

#endif // AZCORE_SCRIPT_SYSTEM_BUS_H
#pragma once
