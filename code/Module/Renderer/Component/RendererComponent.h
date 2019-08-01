#pragma once

#include <AzCore/Component/Component.h>

#include "Renderer/Base/Material.h"
#include "Renderer/Base/RenderNode.h"
#include "Renderer/EBus/RendererComponentBus.h"

namespace Module
{
    class RendererComponent
        : public AZ::Component
        , protected RendererRequestBus::Handler
    {
    public:
        AZ_COMPONENT(RendererComponent, "{BC5AB98F-16F7-4136-A10C-A4A4FD5DCDEE}");

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
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
        }

        static void Reflect(AZ::ReflectContext* context);

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        virtual void Render(size_t subMeshIndex = 0) {}

    protected:
        /////////////////////////////////////////////////////////////////////////////////////
        // RendererRequestBus::Handler
        bool        IsEnabled() const                               override { return m_isEnabled; }
        void        SetEnabled(bool value)                          override { m_isEnabled = value; }

        size_t      GetMaterialCount() const                        override { return m_materials.size(); }
        void        SetMaterialCount(size_t value)                  override { m_materials.resize(value); }

        MaterialPtr GetMaterial(size_t index) const                 override;
        void        SetMaterial(size_t index, MaterialPtr material) override;

        AZ::s16     GetSortingLayer() const                         override { return m_sortingLayer; }
        void        SetSortingLayer(AZ::s16 value)                  override { m_sortingLayer = value; }

        AZ::s16     GetOrderInLayer() const                         override { return m_orderInLayer; }
        void        SetOrderInLayer(AZ::s16 value)                  override { m_orderInLayer = value; }
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        AZStd::vector<AZStd::string> m_materialNames;

        bool                         m_isEnabled    = true;
        AZStd::vector<MaterialPtr>   m_materials;
        AZ::s16                      m_sortingLayer = 0;
        AZ::s16                      m_orderInLayer = 0;

        friend class RendererSystemComponent;
    };
}
