#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    class LevelRequest : public AZ::ComponentBus
    {
    public:
    };

    using LevelRequestBus = AZ::EBus<LevelRequest>;
}