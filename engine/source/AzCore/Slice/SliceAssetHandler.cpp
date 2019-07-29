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
#include <AzCore/Slice/SliceAssetHandler.h>
#include <AzCore/Slice/SliceComponent.h>
#include <AzCore/IO/GenericStreams.h>
#include <AzCore/IO/FileIO.h>
#include <AzCore/Serialization/Util.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Component/Entity.h>

namespace AZ
{
    //=========================================================================
    // SliceAssetHandler
    //=========================================================================
    SliceAssetHandler::SliceAssetHandler(SerializeContext* context)
        : m_filterFlags(0)
    {
        SetSerializeContext(context);
    }

    //=========================================================================
    // CreateAsset
    //=========================================================================
    Data::AssetPtr SliceAssetHandler::CreateAsset(const Data::AssetId& /*id*/, const Data::AssetType& /*type*/)
    {
        return aznew SliceAsset();
    }

    //=========================================================================
    // LoadAssetData
    //=========================================================================
    bool SliceAssetHandler::LoadAssetData(const Data::Asset<Data::AssetData>& asset, IO::GenericStream* stream, const AZ::Data::AssetFilterCB& assetLoadFilterCB)
    {
        SliceAsset* sliceAsset = asset.GetAs<SliceAsset>();
        AZ_Assert(sliceAsset, "This should be a slice asset, as this is the only type we process!");
        if (sliceAsset && m_serializeContext)
        {
            ObjectStream::FilterDescriptor filter(assetLoadFilterCB);
            filter.m_flags = m_filterFlags;

            Entity* sliceEntity = Utils::LoadObjectFromStream<Entity>(*stream, m_serializeContext, filter);
            if (sliceEntity)
            {
                sliceAsset->m_component = sliceEntity->FindComponent<SliceComponent>();
                if (sliceAsset->m_component)
                {
                    // Apply the same filter to any internal object streams generated during instantiation/data-patching.
                    sliceAsset->m_component->m_assetLoadFilterCB = assetLoadFilterCB;
                    sliceAsset->m_component->m_filterFlags = m_filterFlags;

                    sliceAsset->m_component->SetMyAsset(sliceAsset);
                    sliceAsset->m_entity = sliceEntity;
                    sliceAsset->m_component->ListenForAssetChanges();
                    return true;
                }
                else
                {
                    AZ_Error("Slices", false, "Slice entity 0x%08x (%s) doesn't have SliceComponent!", sliceEntity->GetId(), sliceEntity->GetName().c_str());
                    delete sliceEntity;
                }
            }
        }
        return false;
    }

    //=========================================================================
    // DestroyAsset
    //=========================================================================
    void SliceAssetHandler::DestroyAsset(Data::AssetPtr ptr)
    {
        delete ptr;
    }

    //=========================================================================
    // GetSerializeContext
    //=========================================================================.
    SerializeContext* SliceAssetHandler::GetSerializeContext() const
    {
        return m_serializeContext;
    }

    //=========================================================================
    // SetSerializeContext
    //=========================================================================.
    void SliceAssetHandler::SetSerializeContext(SerializeContext* context)
    {
        m_serializeContext = context;

        if (m_serializeContext == nullptr)
        {
            // use the default app serialize context
            EBUS_EVENT_RESULT(m_serializeContext, ComponentApplicationBus, GetSerializeContext);
            if (!m_serializeContext)
            {
                AZ_Error("Slices", false, "SliceAssetHandler: No serialize context provided! We will not be able to process Slice Asset type");
            }
        }
    }

    void SliceAssetHandler::SetFilterFlags(u32 flags)
    {
        m_filterFlags = flags;
    }
} // namespace AZ
