#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <Font/Component/FontSystemComponent.h>

namespace Module
{
    class FontModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(FontModule, "{129864BD-4548-4D72-A62E-EE74A77D31C6}", AZ::Module);
        AZ_CLASS_ALLOCATOR(FontModule, AZ::SystemAllocator, 0);

        FontModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                FontSystemComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<FontSystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(FontModule, Module::FontModule)
