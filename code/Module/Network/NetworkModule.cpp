#include <AzCore/Memory/SystemAllocator.h>
#include <AzCore/Module/Module.h>

#include <Network/Component/NetworkSystemComponent.h>
#include <Network/Component/NetworkClientComponent.h>
#if !defined(AZ_PLATFORM_EMSCRIPTEN)
#include <Network/Component/NetworkServerComponent.h>
#endif

namespace Module
{
    class NetworkModule
        : public AZ::Module
    {
    public:
        AZ_RTTI(NetworkModule, "{FEC4F701-32A5-4633-ABB3-5E4A00B71720}", AZ::Module);
        AZ_CLASS_ALLOCATOR(NetworkModule, AZ::SystemAllocator, 0);

        NetworkModule()
        {
            m_descriptors.insert(m_descriptors.end(), {
                NetworkSystemComponent::CreateDescriptor(),
                NetworkClientComponent::CreateDescriptor(),
#if !defined(AZ_PLATFORM_EMSCRIPTEN)
                NetworkServerComponent::CreateDescriptor(),
#endif
                });
        }

        AZ::ComponentTypeList GetRequiredSystemComponents() const override
        {
            return AZ::ComponentTypeList{
                azrtti_typeid<NetworkSystemComponent>(),
            };
        }
    };
}

AZ_DECLARE_MODULE_CLASS(NetworkModule, Module::NetworkModule)
