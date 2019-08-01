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
#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/GameEntityContextBus.h>
#include "SliceAssetHandler.h"

namespace AZ
{
    class SliceAssetHandler;

    /**
     * Performs management for the Slice (aka Prefab) system.
     */
    class SliceSystemComponent
        : public Component
        , protected GameEntityContextEventBus::Handler
    {
    public:
        AZ_COMPONENT(SliceSystemComponent, "{1712C054-1C84-4995-BA06-AFE99CCFE08B}");
        SliceSystemComponent() = default;
        static void Reflect(ReflectContext* context);

        static void GetProvidedServices(ComponentDescriptor::DependencyArrayType& services);
        static void GetIncompatibleServices(ComponentDescriptor::DependencyArrayType& services);
        static void GetDependentServices(ComponentDescriptor::DependencyArrayType& services);
        static void GetRequiredServices(ComponentDescriptor::DependencyArrayType& required);

    protected:
        SliceSystemComponent(const SliceSystemComponent&) = delete;

        // Component
        void Activate() override;
        void Deactivate() override;

        // GameEntityContextEventBus::Handler
        void OnSliceInstantiated(const Data::AssetId& sliceAssetId, const SliceComponent::SliceInstanceAddress& instance, const SliceInstantiationTicket& ticket) override;
        void OnSliceInstantiationFailed(const Data::AssetId& sliceAssetId, const SliceInstantiationTicket& ticket) override;

        SliceAssetHandler m_assetHandler;
    };
} // namespace AZ