#pragma once

#include "Particle2d/Priv/Particle2dUnit.h"
#include "Particle2d/Priv/Particle2dTypes.h"
#include "AzCore/Math/Random.h"

#include "AzCore/Memory/Memory.h"
#include "AzCore/Memory/SystemAllocator.h"

namespace Particle2d
{
    class Particle2dNode;

    class Particle2DLauncher
    {
	
    public:

		AZ_CLASS_ALLOCATOR(Particle2DLauncher, AZ::SystemAllocator, 0);
        void Setup(Particle2DItem* item, Particle2dNode* node);

        void OnUpdate(float deltaTime);

        void BRUpdate(float deltaTime);

        void Discard();

        void RemoveDeadUnit();

        void AddNewUnits(float deltaTime);

        void CreateNewUnit(const float createTime);

        void GetUnitCreatePosAndDir(Particle2DUnit* unit);

        Particle2DItem* Item;

        Particle2dNode* Node;

        Particle2DLauncherData* Data;

        AZStd::vector<Particle2DItem*> ChildItemPool;

        int CurItemNum = 0;

        bool HasLaunched = false;

        float LastFrameLeftTime = 0.0f;

        static AZ::SimpleLcgRandom m_random;
    };
}
