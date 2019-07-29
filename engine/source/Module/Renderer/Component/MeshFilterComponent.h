#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/std/smart_ptr/shared_ptr.h>

#include "Renderer/EBus/MeshFilterComponentBus.h"
#include "Renderer/Base/Mesh.h"

namespace Module
{
    class MeshFilterComponent
        : public AZ::Component
        , protected MeshFilterRequestBus::Handler
    {
    public:
        AZ_COMPONENT(MeshFilterComponent, "{2C86334D-31B4-42B6-ACDA-A6A9167000B5}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("TransformService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {

        }

        static void Reflect(AZ::ReflectContext* reflection);

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // MeshFilterRequestBus::Handler
        void    SetMesh(MeshPtr mesh) override;
        MeshPtr GetMesh() const       override;
        MeshPtr CloneMesh()           override;
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        AZStd::string m_name;
        MeshPtr       m_mesh = nullptr;
    };
}
