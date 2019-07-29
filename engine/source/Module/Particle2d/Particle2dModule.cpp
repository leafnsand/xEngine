#include "AzCore/Memory/SystemAllocator.h"
#include "AzCore/Module/Module.h"

#include "Particle2d/Component/Particle2dSystemComponent.h"
#include "Particle2d/Component/Particle2dComponent.h"

namespace Module
{
	class Particle2dModule:public AZ::Module
	{
	public:
		AZ_RTTI(Particle2dModule, "{44B3EF25-C0B7-48C8-9BE0-948E827F82FF}", AZ::Module);
		AZ_CLASS_ALLOCATOR(Particle2dModule, AZ::SystemAllocator, 0);

        Particle2dModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                Particle2dSystemComponent::CreateDescriptor(),
                Particle2dComponent::CreateDescriptor(),
            });
        }

		AZ::ComponentTypeList GetRequiredSystemComponents() const override
		{
			return AZ::ComponentTypeList{
                azrtti_typeid<Particle2dSystemComponent>(),
			};
		}
	};
}

AZ_DECLARE_MODULE_CLASS(Particle2dModule, Module::Particle2dModule)
