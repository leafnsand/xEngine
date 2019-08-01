#pragma once

#include "Renderer/Base/Material.h"

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class RendererRequest : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(RendererRequest, "{10E50F92-9771-4BC1-B960-55BEBF0DD2CF}", AZ::ComponentBus)

        virtual bool        IsEnabled() const                               = 0;
        virtual void        SetEnabled(bool value)                          = 0;

        virtual size_t      GetMaterialCount() const                        = 0;
        virtual void        SetMaterialCount(size_t value)                  = 0;

        virtual MaterialPtr GetMaterial(size_t index) const                 = 0;
        virtual void        SetMaterial(size_t index, MaterialPtr material) = 0;

        virtual AZ::s16     GetSortingLayer() const                         = 0;
        virtual void        SetSortingLayer(AZ::s16 value)                  = 0;

        virtual AZ::s16     GetOrderInLayer() const                         = 0;
        virtual void        SetOrderInLayer(AZ::s16 value)                  = 0;
    };

    using RendererRequestBus = AZ::EBus<RendererRequest>;
}