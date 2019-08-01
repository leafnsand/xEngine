#include "Renderer/Component/MeshRendererComponent.h"

#include "Renderer/EBus/MeshFilterComponentBus.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

namespace Module
{
    void MeshRendererComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<MeshRendererComponent>()
                ->SerializerForEmptyClass();
        }
    }

    void MeshRendererComponent::Activate()
    {
        RendererComponent::Activate();

        EBUS_EVENT_ID_RESULT(m_mesh, GetEntityId(), MeshFilterRequestBus, GetMesh);
    }

    void MeshRendererComponent::Deactivate()
    {
        RendererComponent::Deactivate();
    }

    void MeshRendererComponent::Render(size_t subMeshIndex)
    {
        if (m_mesh)
        {
            m_mesh->ApplySubMesh(subMeshIndex);
        }
    }
}
