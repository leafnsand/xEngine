#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class MeshRendererRequest : public AZ::ComponentBus
    {
        
    };

    using MeshRendererRequestBus = AZ::EBus<MeshRendererRequest>;
}