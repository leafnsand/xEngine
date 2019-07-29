#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Slice/SliceBus.h>

#include "Level/EBus/LevelSystemComponentBus.h"

namespace Module
{
    class LevelSystemComponent
        : public AZ::Component
        , protected LevelSystemRequestBus::Handler
    {
    public:
        AZ_COMPONENT(LevelSystemComponent, "{DC179D93-708F-4598-AAC5-43EABB790BEB}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("LevelSystemService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("LevelSystemService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("GameEntityContextService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("GameEntityContextService"));
        }

        static void Reflect(AZ::ReflectContext* reflection);

    protected:
        // AZ::Component
        void Activate() override;
        void Deactivate() override;

        // LevelSystemRequestBus::Handler
        AZ::SliceInstantiationTicket LoadLevel(AZStd::string& path) override;
        void UnloadLevel() override;
        void SetDefaultLevel(AZStd::string path) override;

    private:
        AZStd::string m_defaultLevel;
    };
}
