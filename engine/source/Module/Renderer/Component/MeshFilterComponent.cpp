#include "Renderer/Component/MeshFilterComponent.h"
#include "Window/EBus/WindowSystemComponentBus.h"
#include "Renderer/EBus/RendererSystemComponentBus.h"
#include "Renderer/EBus/RendererComponentBus.h"

#include <AzCore/Component/TickBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <bx/math.h>

namespace Module
{
    void MeshFilterComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<MeshFilterComponent, AZ::Component>()
                ->Field("Name", &MeshFilterComponent::m_name);
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->Class<MeshFilterComponent>("MeshFilterComponent")
                ->Constructor()
                ->Property("m_name", BehaviorValueProperty(&MeshFilterComponent::m_name))
                ;
        }
    }

    void MeshFilterComponent::Activate()
    {
        if (!m_name.empty())
        {
            EBUS_EVENT_RESULT(m_mesh, RendererSystemRequestBus, GetMesh, m_name);
        }

        MeshFilterRequestBus::Handler::BusConnect(GetEntityId());
    }

    void MeshFilterComponent::Deactivate()
    {
        MeshFilterRequestBus::Handler::BusDisconnect();

        m_mesh = nullptr;
    }

    void MeshFilterComponent::SetMesh(MeshPtr mesh)
    {
        m_mesh = mesh;
    }

    MeshPtr MeshFilterComponent::GetMesh() const
    {
        return m_mesh;
    }

    MeshPtr MeshFilterComponent::CloneMesh()
    {
        if (m_mesh->IsShared())
        {
            auto cloneMesh = m_mesh->Clone();
            cloneMesh->MarkUnique();
            SetMesh(cloneMesh);
        }
        return m_mesh;
    }
}
