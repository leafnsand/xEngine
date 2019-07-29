#include "Particle2d/Priv/Particle2dTypes.h"
#include "Particle2d/Priv/Particle2dUnit.h"
#include "Particle2d/Priv/Particle2dItem.h"

#include <AzCore/std/containers/vector.h>
#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/MathUtils.h>

#include "Particle2d/Component/Particle2dComponent.h"

namespace Particle2d
{

    static const uint16_t QuadIndices[] = { 0, 1, 2, 0, 2, 3 };

    AZ::Vector2 Particle2DUnit::VertexXYValue[4]
    {
        AZ::Vector2(-1.0f,-1.0f),
        AZ::Vector2(1.0f,-1.0f),
        AZ::Vector2(1.0f, 1.0f),
        AZ::Vector2(-1.0f, 1.0f)
    };

    AZ::Vector2 Particle2DUnit::VertexUVValue[4]
    {
        AZ::Vector2(0.0f, 0.0f),
        AZ::Vector2(1.0f, 0.0f),
        AZ::Vector2(1.0f, 1.0f),
        AZ::Vector2(0.0f, 1.0f)
    };

    void Particle2DUnit::Setup(Particle2DItem* item)
    {
        this->Item = item;
    }

    void Particle2DUnit::Discard()
    {
        this->Item = nullptr;
    }

    void Particle2DUnit::OnUpdate()
    {
        UpdateTimeInfo();
        UpdateScale();
        UpdateRotation();
        UpdatePosition();
        this->Item->m_fatherLauncher->Node->TotalNeedRenderUnitNum = this->Item->m_fatherLauncher->Node->TotalNeedRenderUnitNum + 1;
    }

    void Particle2DUnit::UpdateTimeInfo()
    {
        this->SelfLiveTime = this->Item->m_componentPtr->m_liveTime - Item->m_createTime;
        this->SelfLifeProgress = this->SelfLiveTime / this->Item->m_lifeTime;
    }

    void Particle2DUnit::UpdateScale()
    {
        if (this->Item->m_fatherLauncher->Data->UseLifeTimeSizeChange || this->ScaleChanged)
        {
            this->CurScale = this->OriginScale *AZ::Lerp(this->Item->m_fatherLauncher->Data->UnitLifetimeStartSize, this->Item->m_fatherLauncher->Data->UnitLifetimeFinSize, this->SelfLifeProgress);
            this->ScaleChanged = true;
        }
    }

    void Particle2DUnit::UpdateRotation()
    {
        if (this->Item->m_fatherLauncher->Data->UseAddRotation || this->RotationChanged)
        {
            this->CurAngle = this->OriginAngle + this->SelfLiveTime * this->AddAngle;
            this->AngleCosValue = cos(this->CurAngle / angleToRaidNum);
            this->AngleSinValue = sin(this->CurAngle / angleToRaidNum);
            if (this->ScaleChanged)
            {
                return;
            }
            this->RotationChanged = true;
        }
    }

    void Particle2DUnit::UpdatePosition()
    {
        if (this->Item->m_fatherLauncher->Data->UseSpeedCurve)
        {
            if (NowSpeedCurveStageIndex < 0)
            {
                float firstStageUseTime = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[1]->Key - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[0]->Key) * this->Item->m_lifeTime;
                NowSpeedCurveStageAcceleration = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[1]->Value - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[0]->Value) / firstStageUseTime;
                NowSpeedCurveStageIndex = 0;
            }
            int updateStageHasMovedNum = 0;
            for (int i = NowSpeedCurveStageIndex + 1; i < this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray.size(); i++)
            {
                if (SelfLifeProgress >= this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Key)
                {
                    NowSpeedCurveStageIndex = i;
                    updateStageHasMovedNum += 1;
                    continue;
                }
                break;
            }
            if (updateStageHasMovedNum > 0)
            {
                for (int i = NowSpeedCurveStageIndex - updateStageHasMovedNum; i < NowSpeedCurveStageIndex; i++)
                {
                    float stageUseTime = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i + 1]->Key - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Key) * this->Item->m_lifeTime;
                    float stageAcceleration = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i + 1]->Value - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Value) / stageUseTime;
                    SpeedCurveHasMoved += this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Value * stageUseTime + 0.5f * stageAcceleration * stageUseTime * stageUseTime;
                }
                float curStageUseTime = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[NowSpeedCurveStageIndex + 1]->Key - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[NowSpeedCurveStageIndex]->Key) * this->Item->m_lifeTime;
                NowSpeedCurveStageAcceleration = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[NowSpeedCurveStageIndex + 1]->Value - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[NowSpeedCurveStageIndex]->Value) / curStageUseTime;
            }
            float nowStageUseTime = SelfLiveTime - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[NowSpeedCurveStageIndex]->Key * this->Item->m_lifeTime;
            float totalMoveLength = SpeedCurveHasMoved + this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[NowSpeedCurveStageIndex]->Value * nowStageUseTime + 0.5f * NowSpeedCurveStageAcceleration * nowStageUseTime * nowStageUseTime;
            CurPos = OriginPos + OriginMoveDir * (totalMoveLength);
        }
        else
        {
            if (this->Item->m_fatherLauncher->Data->Resistance > 0)
            {
                if (SelfLiveTime >= InResistanceMaxMoveTime)
                {
                    CurPos = FinalVecInResistance;
                }
                else
                {
                    CurPos = (OriginSpeed * SelfLiveTime - 0.5f * this->Item->m_fatherLauncher->Data->Resistance * SelfLiveTime * SelfLiveTime) * OriginMoveDir + OriginPos;
                }
            }
            else
            {
                CurPos = OriginSpeed * SelfLiveTime * OriginMoveDir + OriginPos;
            }
        }
        if (UseForce)
        {
            CurPos += 0.5f * ForceVec * SelfLiveTime * SelfLiveTime;
        }
        CurPos *= 100.0f;
    }

    AZ::Vector2 Particle2DUnit::LauncherPartGetUnitPosition(float checkTime)
    {
        AZ::Vector2 targetPos;
        float selfLiveTime = checkTime - this->Item->m_createTime;
        float selfLifeProgress = selfLiveTime / this->Item->m_lifeTime;
        float nowSpeedCurveStageAcceleration = 0;
        int nowSpeedCurveStageIndex = -1;

        if (this->Item->m_fatherLauncher->Data->UseSpeedCurve)
        {
            if (nowSpeedCurveStageIndex < 0)
            {
                float firstStageUseTime = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[1]->Key - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[0]->Key) * this->Item->m_lifeTime;
                nowSpeedCurveStageAcceleration = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[1]->Value - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[0]->Value) / firstStageUseTime;
                nowSpeedCurveStageIndex = 0;
            }
            int updateStageHasMovedNum = 0;
            for (int i = nowSpeedCurveStageIndex + 1; i < this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray.size(); i++)
            {
                if (selfLifeProgress >= this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Key)
                {
                    nowSpeedCurveStageIndex = i;
                    updateStageHasMovedNum += 1;
                    continue;
                }
                break;
            }
            if (updateStageHasMovedNum > 0)
            {
                for (int i = nowSpeedCurveStageIndex - updateStageHasMovedNum; i < nowSpeedCurveStageIndex; i++)
                {
                    float stageUseTime = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i + 1]->Key - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Key) * this->Item->m_lifeTime;
                    float stageAcceleration = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i + 1]->Value - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Value) / stageUseTime;
                    SpeedCurveHasMoved += this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[i]->Value * stageUseTime + 0.5f * stageAcceleration * stageUseTime * stageUseTime;
                }
                float curStageUseTime = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[nowSpeedCurveStageIndex + 1]->Key - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[nowSpeedCurveStageIndex]->Key) * this->Item->m_lifeTime;
                nowSpeedCurveStageAcceleration = (this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[nowSpeedCurveStageIndex + 1]->Value - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[nowSpeedCurveStageIndex]->Value) / curStageUseTime;
            }
            float nowStageUseTime = selfLiveTime - this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[nowSpeedCurveStageIndex]->Key * this->Item->m_lifeTime;
            float totalMoveLength = SpeedCurveHasMoved + this->Item->m_fatherLauncher->Data->SpeedKeyFrameArray[nowSpeedCurveStageIndex]->Value * nowStageUseTime + 0.5f * nowSpeedCurveStageAcceleration * nowStageUseTime * nowStageUseTime;
            targetPos = this->OriginPos + this->OriginMoveDir * (totalMoveLength);
        }
        else
        {
            if (this->Item->m_fatherLauncher->Data->Resistance > 0)
            {
                if (selfLiveTime >= InResistanceMaxMoveTime)
                {
                    targetPos = FinalVecInResistance;
                }
                else
                {
                    targetPos = (OriginSpeed * selfLiveTime - 0.5f * this->Item->m_fatherLauncher->Data->Resistance * selfLiveTime * selfLiveTime) * OriginMoveDir + OriginPos;
                }
            }
            else
            {
                targetPos = OriginSpeed * selfLiveTime * OriginMoveDir + OriginPos;
            }
        }
        if (UseForce)
        {
            targetPos += 0.5f * ForceVec * selfLiveTime * selfLiveTime;
        }
        targetPos *= 100.0f;
        return targetPos;
    }

    void Particle2DUnit::UpdateVertexColor()
    {
        if (this->Item->m_fatherLauncher->Data->UseColorCurve)
        {
            for (int i = NowColorCurveStageIndex + 1; i < this->Item->m_fatherLauncher->Data->ColorKeyFrameArray.size(); i++)
            {
                if (SelfLifeProgress >= this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[i]->Key)
                {
                    NowColorCurveStageIndex = i;
                    continue;
                }
                break;
            }
            float progressValue = (SelfLifeProgress - this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex]->Key) / (this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex + 1]->Key - this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex]->Key);
            CurColor.SetR(OriginColor.GetR() * AZ::Lerp(this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex]->Color.GetR(), this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex + 1]->Color.GetR(), progressValue));
            CurColor.SetG(OriginColor.GetG() * AZ::Lerp(this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex]->Color.GetG(), this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex + 1]->Color.GetG(), progressValue));
            CurColor.SetB(OriginColor.GetB() * AZ::Lerp(this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex]->Color.GetB(), this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex + 1]->Color.GetB(), progressValue));
            CurColor.SetA(OriginColor.GetA() * AZ::Lerp(this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex]->Color.GetA(), this->Item->m_fatherLauncher->Data->ColorKeyFrameArray[NowColorCurveStageIndex + 1]->Color.GetA(), progressValue));
        }
        else
        {
            CurColor = OriginColor;
        }
    }

    void Particle2DUnit::UpdateVertexXY()
    {
        if (ScaleChanged || RotationChanged)
        {
            for (int i = 0; i < 4; i++)
            {
                float x = this->Item->m_fatherLauncher->Data->UnitSizeX * CurScale * Particle2DUnit::VertexXYValue[i].GetX() / 2.0f;
                float y = this->Item->m_fatherLauncher->Data->UnitSizeY * CurScale * Particle2DUnit::VertexXYValue[i].GetY() / 2.0f;
                SelfOffsetArr[i].SetX(AngleCosValue * x - AngleSinValue *y);
                SelfOffsetArr[i].SetY(AngleSinValue *x + AngleCosValue *y);
                CurVertexDataArr[i].SetX(CurPos.GetX() + SelfOffsetArr[i].GetX());
                CurVertexDataArr[i].SetY(CurPos.GetY() + SelfOffsetArr[i].GetY());
            }
            ScaleChanged = false;
            RotationChanged = false;
        }
        else
        {
            for (int i = 0; i < 4; i++)
            {
                CurVertexDataArr[i].SetX(CurPos.GetX() + SelfOffsetArr[i].GetX());
                CurVertexDataArr[i].SetY(CurPos.GetY() + SelfOffsetArr[i].GetY());
            }
        }
    }

    void Particle2DUnit::UpdateVertexUV(float deltaTime)
    {
        if (this->Item->m_fatherLauncher->Data->UseFrame)
        {
            int nowShowFrameIndex = (int)(SelfLiveTime * this->Item->m_fatherLauncher->Data->UnitFrameFPS) + OriginFrameIndex;
            if (nowShowFrameIndex != CurFrameIndex)
            {
                NeedUpdateFrameUV = true;
                CurFrameColumnIndex = nowShowFrameIndex % this->Item->m_fatherLauncher->Data->UnitFrameColumnNum;

                CurFrameRowIndex = (nowShowFrameIndex - CurFrameColumnIndex) / this->Item->m_fatherLauncher->Data->UnitFrameRowNum;
            }
        }
        if (NeedUpdateFrameUV)
        {
            for (int i = 0; i < 4; i++)
            {
                UpdateVertexUV(i);
            }
        }
       
        if (this->Item->m_fatherLauncher->Data->UseUVScroll && this->Item->m_componentPtr->m_isRunning)
        {
            for (int i = 0; i < 4; i++)
            {
                CurVertexDataArr[i].SetZ(CurVertexDataArr[i].GetZ() + deltaTime * this->Item->m_fatherLauncher->Data->XScrollSpeed);
                CurVertexDataArr[i].SetW(CurVertexDataArr[i].GetW() + deltaTime * this->Item->m_fatherLauncher->Data->YScrollSpeed);
            }
        }
    }

    void Particle2DUnit::UpdateVertexUV(int vertexIndex)
    {
        switch (vertexIndex)
        {
        case 0:
            CurVertexDataArr[vertexIndex].SetZ((float)CurFrameColumnIndex / this->Item->m_fatherLauncher->Data->UnitFrameColumnNum);
            CurVertexDataArr[vertexIndex].SetW((float)(CurFrameRowIndex + 1) / this->Item->m_fatherLauncher->Data->UnitFrameRowNum);
            break;
        case 1:
            CurVertexDataArr[vertexIndex].SetZ((float)(CurFrameColumnIndex + 1) / this->Item->m_fatherLauncher->Data->UnitFrameColumnNum);
            CurVertexDataArr[vertexIndex].SetW((float)(CurFrameRowIndex + 1) / this->Item->m_fatherLauncher->Data->UnitFrameRowNum);
            break;
        case 2:
            CurVertexDataArr[vertexIndex].SetZ((float)(CurFrameColumnIndex + 1) / this->Item->m_fatherLauncher->Data->UnitFrameColumnNum);
            CurVertexDataArr[vertexIndex].SetW((float)CurFrameRowIndex / this->Item->m_fatherLauncher->Data->UnitFrameRowNum);
            break;
        case 3:
            CurVertexDataArr[vertexIndex].SetZ((float)CurFrameColumnIndex / this->Item->m_fatherLauncher->Data->UnitFrameColumnNum);
            CurVertexDataArr[vertexIndex].SetW((float)CurFrameRowIndex / this->Item->m_fatherLauncher->Data->UnitFrameRowNum);
            NeedUpdateFrameUV = false;
            break;
        }
    }

    void Particle2DUnit::CalculateVertexData(int vertexBufferStartIndex, float deltaTime)
    {
        UpdateVertexColor();
        UpdateVertexXY();
        UpdateVertexUV(deltaTime);
        AZStd::vector<Particle2d::VertexData>& buffer = this->Item->m_componentPtr->m_vertexBuffer;
        for (int i = 0; i < 4; i++)
        {
            int targetIndex = i + vertexBufferStartIndex;
            buffer[targetIndex].x = this->CurVertexDataArr[i].GetX() / 100.0f;
            buffer[targetIndex].y = this->CurVertexDataArr[i].GetY() / 100.0f;
            buffer[targetIndex].u = this->CurVertexDataArr[i].GetZ() ;
            buffer[targetIndex].v = this->CurVertexDataArr[i].GetW() ;
            buffer[targetIndex].r = this->CurColor.GetR();
            buffer[targetIndex].g = this->CurColor.GetG();
            buffer[targetIndex].b = this->CurColor.GetB();
            buffer[targetIndex].a = this->CurColor.GetA();
        }

        for (int i = 0; i < 6; i++)
        {
            this->Item->m_componentPtr->m_indexBuffer.push_back(vertexBufferStartIndex + QuadIndices[i]);
        }
        this->Item->m_componentPtr->m_indexCount += 6;
    }
}