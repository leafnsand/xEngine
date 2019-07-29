#pragma once

#include "AzCore/Component/ComponentBus.h"

namespace Module
{
    class Particle2dRequest : public AZ::ComponentBus
	{
    public:

        virtual bool IsValid() const = 0;

        virtual void Stop() = 0;

        virtual void Restart() = 0;

        virtual void Pause() = 0;

        virtual void Resume() = 0;

        virtual void Clean() = 0;
	};

    using Particle2dRequestBus = AZ::EBus<Particle2dRequest>;
}