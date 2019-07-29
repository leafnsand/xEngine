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

#include <AzCore/Memory/MemoryComponent.h>
#include <AzCore/Math/Crc.h>

#include <AzCore/Memory/PoolAllocator.h>

#include <AzCore/Serialization/SerializeContext.h>
namespace AZ
{
    //=========================================================================
    // MemoryComponent
    // [5/29/2012]
    //=========================================================================
    MemoryComponent::MemoryComponent()
    {
        m_isPoolAllocator = true;
        m_isThreadPoolAllocator = true;

        m_createdPoolAllocator = false;
        m_createdThreadPoolAllocator = false;
    }

    //=========================================================================
    // ~MemoryComponent
    // [5/29/2012]
    //=========================================================================
    MemoryComponent::~MemoryComponent()
    {
        // Normally we will just call destroy destroy in the deactivate function
        // and create in activate. But memory component is special that
        // it must be operational after Init so all parts of the engine can be operational.
        // This is why we must check the destructor (which is symmetrical to Init() anyway)
        if (m_createdThreadPoolAllocator && AZ::AllocatorInstance<AZ::ThreadPoolAllocator>::IsReady())
        {
            AZ::AllocatorInstance<AZ::ThreadPoolAllocator>::Destroy();
        }
        if (m_createdPoolAllocator && AZ::AllocatorInstance<AZ::PoolAllocator>::IsReady())
        {
            AZ::AllocatorInstance<AZ::PoolAllocator>::Destroy();
        }
    }

    //=========================================================================
    // Init
    // [5/29/2012]
    //=========================================================================
    void MemoryComponent::Init()
    {
        // IMPORTANT: The SystemAllocator is already operational

        // TODO: add the setting to the serialization layer
        if (m_isPoolAllocator)
        {
            AZ::AllocatorInstance<AZ::PoolAllocator>::Create();
            m_createdPoolAllocator = true;
        }
        if (m_isThreadPoolAllocator)
        {
            AZ::AllocatorInstance<AZ::ThreadPoolAllocator>::Create();
            m_createdThreadPoolAllocator = true;
        }
    }

    //=========================================================================
    // Activate
    // [5/29/2012]
    //=========================================================================
    void MemoryComponent::Activate()
    {
    }

    //=========================================================================
    // Deactivate
    // [5/29/2012]
    //=========================================================================
    void MemoryComponent::Deactivate()
    {
    }

    //=========================================================================
    // GetProvidedServices
    //=========================================================================
    void MemoryComponent::GetProvidedServices(ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("MemoryService", 0x5c4d473c));
    }

    //=========================================================================
    // GetIncompatibleServices
    //=========================================================================
    void MemoryComponent::GetIncompatibleServices(ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("MemoryService", 0x5c4d473c));
    }

    //=========================================================================
    // Reflect
    //=========================================================================
    void MemoryComponent::Reflect(ReflectContext* context)
    {
        if (SerializeContext* serializeContext = azrtti_cast<SerializeContext*>(context))
        {
            serializeContext->Class<MemoryComponent, AZ::Component>()
                ->Field("isPoolAllocator", &MemoryComponent::m_isPoolAllocator)
                ->Field("isThreadPoolAllocator", &MemoryComponent::m_isThreadPoolAllocator)
                ;
        }
    }
} // namespace AZ


#endif // #ifndef AZ_UNITY_BUILD
