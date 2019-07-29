#pragma once

#include <AzCore/Component/Component.h>

#include "Renderer/Base/Material.h"

#include "Level/EBus/LevelComponentBus.h"

namespace Module
{
    class LevelComponent
        : public AZ::Component
        , protected LevelRequestBus::Handler
    {
    public:
        AZ_COMPONENT(LevelComponent, "{A9F77249-F9AF-41C5-8371-23649B991A21}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("LevelService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("LevelService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {

        }

        static void Reflect(AZ::ReflectContext* reflection);

        LevelComponent();
        LevelComponent(const AZStd::string& name);

        // AZ::Component
        void Init() override;
        void Activate() override;
        void Deactivate() override;

    protected:
        
    private:
        AZStd::string m_skyBoxName = "";
        AZStd::shared_ptr<Material> m_matSkyBox = nullptr;
        int m_divisions = 0;
    };
}
