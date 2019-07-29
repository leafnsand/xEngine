#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <Window/Component/WindowSystemComponent.h>

namespace Module
{
    class WindowModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(WindowModule, "{F03B2B08-B43C-4EDA-8011-B6CA687A3FC9}", AZ::Module);
        AZ_CLASS_ALLOCATOR(WindowModule, AZ::SystemAllocator, 0);

        WindowModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                WindowSystemComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<WindowSystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(WindowModule, Module::WindowModule)
