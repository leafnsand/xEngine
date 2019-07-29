#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <FairyGUI/Component/FairySystemComponent.h>
#include "Component/FairyPanelComponent.h"

namespace Module
{
    class FairyModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(FairyModule, "{0D7230ED-D6A9-4ED9-AFC3-444FD8C0846B}", AZ::Module);
        AZ_CLASS_ALLOCATOR(FairyModule, AZ::SystemAllocator, 0);

        FairyModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                FairySystemComponent::CreateDescriptor(),
                FairyPanelComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<FairySystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(FairyModule, Module::FairyModule)
