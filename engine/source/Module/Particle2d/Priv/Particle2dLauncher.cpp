#include "Particle2d/Priv/Particle2dLauncher.h"
#include "Particle2d/Priv/Particle2dNode.h"
#include "Particle2d/Priv/Particle2dItem.h"
#include "AzCore/std/containers/fixed_list.h"
#include "AzCore/Math/Random.h"
#include "AzCore/Math/MathUtils.h"
#include "AzCore/Math/Vector2.h"
#include <math.h>

#include "Particle2d/Component/Particle2dComponent.h"
#include "Component/Particle2dSystemComponent.h"

namespace Particle2d
{
    AZ::SimpleLcgRandom Particle2DLauncher::m_random;

    void Particle2DLauncher::Setup(Particle2DItem* item, Particle2dNode* node)
    {
        this->Item = item;
        this->Node = node;
        this->Data = node->m_dataPtr;
        this->CurItemNum = 0;
        this->ChildItemPool.clear();
    }

    void Particle2DLauncher::OnUpdate(float deltaTime)
    {
        if (this->Item->m_componentPtr->m_liveTime < this->Item->m_createTime + this->Data->DelayTime)
        {
            return;
        }
        if (!this->HasLaunched)
        {
            this->LastFrameLeftTime = this->Item->m_componentPtr->m_liveTime - this->Item->m_createTime - this->Data->DelayTime;
        }
        
        this->RemoveDeadUnit();
        this->AddNewUnits(deltaTime);
        for (auto& child : this->ChildItemPool) 
        {
            child->OnUpdate(deltaTime);
        }
    }

    void Particle2DLauncher::RemoveDeadUnit()
    {
        if (this->ChildItemPool.empty())
        {
            return;
        }
        int a = this->ChildItemPool.size();
        for (int i = this->ChildItemPool.size() - 1; i >= 0; i--)
        {
            if (!this->ChildItemPool[i]->CheckLive())
            {
                this->ChildItemPool[i]->Discard();

                this->ChildItemPool.erase(&this->ChildItemPool[i]);
            }
        }
    }

    void Particle2DLauncher::Discard()
    {
        for (int i = this->ChildItemPool.size() - 1; i >= 0; i--)
        {
            this->ChildItemPool[i]->Discard();
            this->ChildItemPool.erase(&this->ChildItemPool[i]);
        }

        this->Item = nullptr;
        this->Node = nullptr;
        this->Data = nullptr;
        this->HasLaunched = false;
        this->LastFrameLeftTime = 0.0f;
    }

    void Particle2DLauncher::AddNewUnits(float deltaTime)
    {
        int totalAddUnitNum = 0;
        float totalNeedCheckTime = this->LastFrameLeftTime;
        if (this->Item->m_componentPtr->m_isRunning)
        {
            totalNeedCheckTime += deltaTime;
        }
     
        if (totalNeedCheckTime < 0)
        {
            return;
        }
        if (!HasLaunched)
        {
            this->HasLaunched = true;
            this->CreateNewUnit(this->Item->m_createTime + this->Data->DelayTime);
        }
        while (totalNeedCheckTime >= this->Data->PerUnitTime)
        {
            totalNeedCheckTime -= this->Data->PerUnitTime;
            if ((this->Item->m_componentPtr->m_liveTime - totalNeedCheckTime) > (this->Item->m_createTime + this->Data->DelayTime + this->Data->Duration))
            {
                if (!this->Data->Loop)
                {
                    totalNeedCheckTime = 0;
                    break;
                }
            }
            CreateNewUnit(this->Item->m_componentPtr->m_liveTime - totalNeedCheckTime);
        }
        this->LastFrameLeftTime = totalNeedCheckTime;
    }

    void Particle2DLauncher::CreateNewUnit(float createTime)
    {
   
        if (this->ChildItemPool.size() == this->Data->MaxParticlesNum) 
        {
            return;
        }
       
        float lifeTime = this->Data->UnitLifeTimeRandom ? (Particle2DLauncher::m_random.GetRandomFloat() * (this->Data->UnitMaxLifeTime - this->Data->UnitMinLifetime) + this->Data->UnitMinLifetime) : this->Data->UnitLifeTime;
        if ((createTime + lifeTime) < this->Item->m_componentPtr->m_liveTime)
        {
            return;
        }
        // auto item = &this->ChildItemPool.Add();
        Particle2DItem* item = aznew Particle2DItem();
        this->ChildItemPool.push_back(item);
        item->Setup(Item->m_componentPtr, Node, this, createTime, lifeTime);
        GetUnitCreatePosAndDir(item->m_unit);
        if (this->Item->m_unit == nullptr)
        {
            item->m_unit->OriginPos = this->Data->Offset *2 + item->m_unit->OriginPos;
        }
        else
        {
            item->m_unit->OriginPos =this->Item->m_unit->LauncherPartGetUnitPosition(createTime) +item->m_unit->OriginPos;
        }
        item->m_unit->UseForce = this->Data->useForce;
        item->m_unit->ForceVec.SetX(sin(this->Data->ForceDir / angleToRaidNum) * this->Data->ForceValue);
        item->m_unit->ForceVec.SetY(cos(this->Data->ForceDir / angleToRaidNum) * this->Data->ForceValue);
        if (this->Data->UseSpeedCurve)
        {
            // fix me
        }
        else
        {
            item->m_unit->OriginSpeed = this->Data->UnitSpeedRandom ? (Particle2DLauncher::m_random.GetRandomFloat() * (this->Data->UnitMaxSpeed - this->Data->UnitMinSpeed) + this->Data->UnitMinSpeed) : this->Data->UnitSpeed;
            if (this->Data->Resistance > 0)
            {
                item->m_unit->InResistanceMaxMoveTime = abs(item->m_unit->OriginSpeed) / this->Data->Resistance;
                item->m_unit->FinalVecInResistance = item->m_unit->OriginMoveDir * (item->m_unit->OriginSpeed *item->m_unit->InResistanceMaxMoveTime - 0.5f * this->Data->Resistance * item->m_unit->InResistanceMaxMoveTime* item->m_unit->InResistanceMaxMoveTime) + item->m_unit->OriginPos;
            }
        }
        item->m_unit->OriginAngle = this->Data->UnitRotationRandom ? (this->Data->UnitMinRotation + Particle2DLauncher::m_random.GetRandomFloat() * (this->Data->UnitMaxRotation - this->Data->UnitMinRotation)) : this->Data->UnitRotation;
        item->m_unit->AddAngle = this->Data->UnitAddRotationRandom ? (this->Data->UnitMinAddRotation + Particle2DLauncher::m_random.GetRandomFloat() * (this->Data->UnitMaxAddRotation - this->Data->UnitMinAddRotation)) : this->Data->UnitAddRotation;
        item->m_unit->OriginScale = this->Data->UnitSizeRandom ? (this->Data->UnitMinSize + Particle2DLauncher::m_random.GetRandomFloat() * (this->Data->UnitMaxSize - this->Data->UnitMinSize)) : 1.0f;
        float colorRandomValue = Particle2DLauncher::m_random.GetRandomFloat();
        item->m_unit->OriginColor.SetR(this->Data->UnitColorRandom ? AZ::VectorFloat(AZ::Lerp(float(this->Data->UnitMinColor.GetR()), float(this->Data->UnitMaxColor.GetR()), colorRandomValue)) : this->Data->UnitColor.GetR());
        item->m_unit->OriginColor.SetG(this->Data->UnitColorRandom ? AZ::VectorFloat(AZ::Lerp(float(this->Data->UnitMinColor.GetG()), float(this->Data->UnitMaxColor.GetG()), colorRandomValue)) : this->Data->UnitColor.GetG());
        item->m_unit->OriginColor.SetB(this->Data->UnitColorRandom ? AZ::VectorFloat(AZ::Lerp(float(this->Data->UnitMinColor.GetB()), float(this->Data->UnitMaxColor.GetB()), colorRandomValue)) : this->Data->UnitColor.GetB());
        item->m_unit->OriginColor.SetA(this->Data->UnitColorRandom ? AZ::VectorFloat(AZ::Lerp(float(this->Data->UnitMinColor.GetA()), float(this->Data->UnitMaxColor.GetA()), colorRandomValue)) : this->Data->UnitColor.GetA());
        if (this->Data->UnitFrameRandom)
        {
            item->m_unit->OriginFrameIndex = (int)(Particle2DLauncher::m_random.GetRandomFloat() * (this->Data->UnitFrameRowNum * this->Data->UnitFrameColumnNum));       //(int)()//                        Math::n_rand(0, this->Data->UnitFrameRowNum * this->Data->UnitFrameColumnNum);
        }
    }

    void Particle2DLauncher::BRUpdate(float deltaTime)
    {
        for (auto& child : this->ChildItemPool)
        {
            child->BRUpdate(deltaTime);
            if (child->m_unit != nullptr)
            {
                int startIndex = this->Node->GetVertexBufferIndex();
                child->m_unit->CalculateVertexData(startIndex,deltaTime);
            }
        }
    }

    void Particle2DLauncher::GetUnitCreatePosAndDir(Particle2DUnit* unit)
    {
        // AZ::SimpleLcgRandom random;
        float launcher{ 0 };
        float lerpRandomValue{ 0 };
        AZ::Vector2 launcherLeftPoint;
        AZ::Vector2 launcherRightPoint;
        // float randomValue = 0;
        // EBUS_EVENT_RESULT(randomValue, Module::Particle2dSystemRequestBus, GetRandomValue);
        switch (this->Data->ParticleLauncherType)
        {
        case Circlue:
            launcher = Particle2DLauncher::m_random.GetRandomFloat() * 360.0f;
            launcherLeftPoint.SetX(sin((launcher - 90.0f) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherLeftPoint.SetY(cos((launcher - 90.0f) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherRightPoint.SetX(sin((launcher + 90.0f) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherRightPoint.SetY(cos((launcher + 90.0f) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            lerpRandomValue = Particle2DLauncher::m_random.GetRandomFloat();
            unit->OriginPos.SetX(AZ::Lerp(launcherLeftPoint.GetX(), launcherRightPoint.GetX(), lerpRandomValue));
            unit->OriginPos.SetY(AZ::Lerp(launcherLeftPoint.GetY(), launcherRightPoint.GetY(), lerpRandomValue));
            if (unit->Item->m_fatherLauncher->Data->CirculeNotRandomDir)
            {
                launcher = unit->Item->m_fatherLauncher->Data->LauncherShootAngle + 360.0f + (Particle2DLauncher::m_random.GetRandomFloat() * unit->Item->m_fatherLauncher->Data->LauncherShootRandomNum * 2 - unit->Item->m_fatherLauncher->Data->LauncherShootRandomNum);
            }
            else
            {
                launcher = Particle2DLauncher::m_random.GetRandomFloat() * 360.0f;
            }
            unit->OriginMoveDir.SetX(sin(launcher / angleToRaidNum));
            unit->OriginMoveDir.SetY(cos(launcher / angleToRaidNum));
            break;
        case Cone:
            launcher = unit->Item->m_fatherLauncher->Data->LauncherShootAngle + 360.0f;
            launcherLeftPoint.SetX(sin((launcher - 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherLeftPoint.SetY(cos((launcher - 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherRightPoint.SetX(sin((launcher + 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherRightPoint.SetY(cos((launcher + 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            lerpRandomValue = Particle2DLauncher::m_random.GetRandomFloat();
            unit->OriginPos.SetX(AZ::Lerp(launcherLeftPoint.GetX(), launcherRightPoint.GetX(), lerpRandomValue));
            unit->OriginPos.SetY( AZ::Lerp(launcherLeftPoint.GetY(), launcherRightPoint.GetY(), lerpRandomValue));
            launcher = launcher + (Particle2DLauncher::m_random.GetRandomFloat() * unit->Item->m_fatherLauncher->Data->LauncherShootRandomNum * 2 - unit->Item->m_fatherLauncher->Data->LauncherShootRandomNum);
            unit->OriginMoveDir.SetX(sin(launcher / angleToRaidNum));
            unit->OriginMoveDir.SetY(cos(launcher / angleToRaidNum));
            break;
        case Point:
          
            launcher = Particle2DLauncher::m_random.GetRandomFloat() * 360.0f;
            unit->OriginMoveDir.SetX(sin(launcher / angleToRaidNum));
            unit->OriginMoveDir.SetY(cos(launcher / angleToRaidNum));
            break;
        case Square:
            launcher = unit->Item->m_fatherLauncher->Data->LauncherShootAngle;
            launcherLeftPoint.SetX(sin((launcher - 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherLeftPoint.SetY(cos((launcher - 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherRightPoint.SetX(sin((launcher + 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcherRightPoint.SetY(cos((launcher + 90) / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            lerpRandomValue = Particle2DLauncher::m_random.GetRandomFloat();
            unit->OriginPos.SetX(AZ::Lerp(launcherLeftPoint.GetX(), launcherRightPoint.GetX(), lerpRandomValue));
            unit->OriginPos.SetY(AZ::Lerp(launcherLeftPoint.GetY(), launcherRightPoint.GetY(), lerpRandomValue));
            unit->OriginMoveDir.SetX(sin(launcher / angleToRaidNum));
            unit->OriginMoveDir.SetY(cos(launcher / angleToRaidNum));
            break;
        case Circule2Center:
            launcher = Particle2DLauncher::m_random.GetRandomFloat() * 360.0f;
            unit->OriginPos.SetX(sin(launcher / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            unit->OriginPos.SetY(cos(launcher / angleToRaidNum) * unit->Item->m_fatherLauncher->Data->LauncherShootLength * 0.5f);
            launcher += 180;
            unit->OriginMoveDir.SetX(sin(launcher / angleToRaidNum));
            unit->OriginMoveDir.SetY(cos(launcher / angleToRaidNum));
            break;
        }
        unit->Angle = launcher;
    }
}
