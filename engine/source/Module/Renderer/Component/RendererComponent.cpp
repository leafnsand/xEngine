#include "Renderer/Component/RendererComponent.h"

#include "Renderer/EBus/RendererSystemComponentBus.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/std/smart_ptr/make_shared.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <bgfx/bgfx.h>

namespace Module
{
    void RendererComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<RendererComponent, AZ::Component>()
                ->Field("MaterialNames", &RendererComponent::m_materialNames)
                ->Field("SortingLayer", &RendererComponent::m_sortingLayer)
                ->Field("OrderInLayer", &RendererComponent::m_orderInLayer);
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(context))
        {
            behaviorContext->EBus<RendererRequestBus>("RendererRequestBus")
                ->Event("IsEnabled", &RendererRequestBus::Events::IsEnabled)
                ->Event("SetEnabled", &RendererRequestBus::Events::SetEnabled)
                ->Event("GetMaterial", &RendererRequestBus::Events::GetMaterial)
                ->Event("SetMaterial", &RendererRequestBus::Events::SetMaterial)
                ->Event("GetSortingLayer", &RendererRequestBus::Events::GetSortingLayer)
                ->Event("SetSortingLayer", &RendererRequestBus::Events::SetSortingLayer)
                ->Event("GetOrderInLayer", &RendererRequestBus::Events::GetOrderInLayer)
                ->Event("SetOrderInLayer", &RendererRequestBus::Events::SetOrderInLayer)
                ;

            behaviorContext->Class<RendererComponent>("RendererComponent")
                ->Constructor()
                ->Property("materialNames", BehaviorValueProperty(&RendererComponent::m_materialNames))
                ->Property("sortingLayer", BehaviorValueProperty(&RendererComponent::m_sortingLayer))
                ->Property("orderInLayer", BehaviorValueProperty(&RendererComponent::m_orderInLayer));
        }
        
    }

    void RendererComponent::Activate()
    {
        m_materials.resize(m_materialNames.size());

        for (size_t index = 0; index < m_materialNames.size(); ++index)
        {
            auto& material = m_materials.at(index);
            EBUS_EVENT_RESULT(material, RendererSystemRequestBus, GetMaterial, m_materialNames.at(index));
        }

        RendererRequestBus::Handler::BusConnect(GetEntityId());
    }

    void RendererComponent::Deactivate()
    {
        m_materials.clear();

        RendererRequestBus::Handler::BusDisconnect();
    }

    MaterialPtr RendererComponent::GetMaterial(size_t index) const
    {
        if (index < m_materials.size())
        {
            return m_materials.at(index);
        }
        return nullptr;
    }

    void RendererComponent::SetMaterial(size_t index, MaterialPtr material)
    {
        if (index < m_materials.size())
        {
            m_materials[index] = material;
        }
    }
}
