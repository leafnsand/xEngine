#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "Gfx/Component/CanvasComponent.h"

namespace Module
{
    class GfxModule :public AZ::Module
    {
    public:
        AZ_RTTI(GfxModule, "{801A8414-A8C8-4664-B457-809A7705E9ED}", AZ::Module);
        AZ_CLASS_ALLOCATOR(GfxModule, AZ::SystemAllocator, 0);

        GfxModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                CanvasComponent::CreateDescriptor(),
                });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(GfxModule, Module::GfxModule)