#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <Level/Component/LevelSystemComponent.h>
#include <Level/Component/LevelComponent.h>

namespace Module
{
    class LevelModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(LevelModule, "{359AC1B1-F035-4C41-8638-E5A6A533D09D}", AZ::Module);
        AZ_CLASS_ALLOCATOR(LevelModule, AZ::SystemAllocator, 0);

        LevelModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                LevelSystemComponent::CreateDescriptor(),
                LevelComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<LevelSystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(LevelModule, Module::LevelModule)
