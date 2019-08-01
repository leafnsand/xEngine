#pragma once

#include "Renderer/Base/Mesh.h"

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class MeshFilterRequest : public AZ::ComponentBus
    {
    public:
        virtual void    SetMesh(MeshPtr mesh) = 0;
        virtual MeshPtr GetMesh() const       = 0;
        virtual MeshPtr CloneMesh()           = 0;
    };

    using MeshFilterRequestBus = AZ::EBus<MeshFilterRequest>;
}
