#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include "Physics2d/Component/Collider2dComponent.h"
#include "Physics2d/Component/Physics2dSystemComponent.h"
#include "Physics2d/Component/BoxCollider2dComponent.h"
#include "Physics2d/Component/CircleCollider2dComponent.h"
#include "Physics2d/Component/EdgeCollider2dComponent.h"
#include "Physics2d/Component/Rigidbody2dComponent.h"


namespace Module
{
    class Physics2dModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(Physics2dModule, "{DA3A3D59-0244-406E-8BB9-6B221F891CDA}", AZ::Module);
        AZ_CLASS_ALLOCATOR(Physics2dModule, AZ::SystemAllocator, 0);

        Physics2dModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                Collider2dComponent::CreateDescriptor(),
                Physics2dSystemComponent::CreateDescriptor(),
                BoxCollider2dComponent::CreateDescriptor(),
                CircleCollider2dComponent::CreateDescriptor(),
                EdgeCollider2dComponent::CreateDescriptor(),
                Rigidbody2dComponent::CreateDescriptor(),
            });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<Physics2dSystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(Physics2dModule, Module::Physics2dModule)
