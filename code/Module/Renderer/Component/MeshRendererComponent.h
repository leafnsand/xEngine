#pragma once

#include "Renderer/Component/RendererComponent.h"

#include "Renderer/Base/Mesh.h"

#include "Renderer/EBus/MeshRendererComponentBus.h"

namespace Module
{
    class MeshRendererComponent
        : public RendererComponent
        , protected MeshRendererRequestBus::Handler
    {
    public:
        AZ_COMPONENT(MeshRendererComponent, "{E0FCB0EB-CA85-4468-B514-1CA2FD2481CA}", RendererComponent);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("RendererService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("RendererService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("MeshProviderService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("MeshProviderService"));
        }

        static void Reflect(AZ::ReflectContext* context);

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        void Render(size_t subMeshIndex) override;

    private:
        MeshPtr m_mesh;
    };
}
