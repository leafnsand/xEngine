#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <Spine/Component/SpineComponent.h>

namespace Module
{
    class SpineModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(SpineModule, "{393B7EEE-CB0B-4E6D-B320-CE284017C030}", AZ::Module);
        AZ_CLASS_ALLOCATOR(SpineModule, AZ::SystemAllocator, 0);

        SpineModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                SpineComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(SpineModule, Module::SpineModule)
