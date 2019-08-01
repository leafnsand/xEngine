#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Input/Events/InputChannelEventListener.h>

#include <AzCore/InputEvent/InputEventSystemComponentBus.h>

namespace AZ
{
    class InputEventSystemComponent
        : public AZ::Component
        , protected InputEventSystemRequestBus::Handler
        , public AZ::InputChannelEventListener
    {
    public:
        AZ_COMPONENT(InputEventSystemComponent, "{B1400000-62FA-40BC-89E2-1B7BB3EF6359}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("InputEventSystemService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("InputEventSystemService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("InputSystemService", 0x5438d51a));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("InputSystemService", 0x5438d51a));
        }

        static void Reflect(AZ::ReflectContext* reflection);

        InputEventSystemComponent();
        InputEventSystemComponent(const InputEventSystemComponent&) = delete;
        ~InputEventSystemComponent() override = default;

    protected:
        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

        //InputChannelEventListener
        bool OnInputChannelEventFiltered(const AZ::InputChannel& inputChannel) override;
    };
}
