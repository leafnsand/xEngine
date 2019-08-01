#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "Renderer/Component/RendererSystemComponent.h"
#include "Renderer/Component/MeshFilterComponent.h"
#include "Renderer/Component/RendererComponent.h"
#include "Renderer/Component/CameraComponent.h"
#include "Renderer/Component/SpriteRendererComponent.h"

namespace Module
{
    class RendererModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(RendererModule, "{203B8808-913D-4B02-9876-A2290E79DEFB}", AZ::Module);
        AZ_CLASS_ALLOCATOR(RendererModule, AZ::SystemAllocator, 0);

        RendererModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                RendererSystemComponent::CreateDescriptor(),
                MeshFilterComponent::CreateDescriptor(),
                RendererComponent::CreateDescriptor(),
                CameraComponent::CreateDescriptor(),
                SpriteRendererComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<RendererSystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(RendererModule, Module::RendererModule)
