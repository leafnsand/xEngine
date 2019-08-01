#pragma once

#include "Particle2d/Priv/Particle2dLauncher.h"
#include "Particle2d/Priv/Particle2dNode.h"

#include "AzCore/Memory/Memory.h"
#include "AzCore/Memory/SystemAllocator.h"

namespace Module {
    class Particle2dComponent;
}

namespace Particle2d
{

    class Particle2DItem
    {
	
    public:
		AZ_CLASS_ALLOCATOR(Particle2DItem, AZ::SystemAllocator, 0);

        void Setup(Module::Particle2dComponent* componentPtr,Particle2dNode* nodePtr, Particle2DLauncher* LauncherPtr, float createTime, float lifeTime = 86400);

        void Setup(Module::Particle2dComponent* componentPtr,float createTime, float lifeTime = 86400);

        void Discard();

        bool CheckLive();

        void OnUpdate(float deltaTime);

        void BRUpdate(float deltaTime);

        float m_createTime = 0;

        float m_lifeTime = 0;

        float m_deadTime = 0;

        AZStd::vector<Particle2DLauncher*> m_launcherVector;

        Particle2DLauncher* m_fatherLauncher = nullptr;

        Particle2DUnit* m_unit = nullptr;
        
        Module::Particle2dComponent* m_componentPtr;
    };
}
