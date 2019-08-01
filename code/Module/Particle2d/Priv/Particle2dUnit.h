#pragma once

#include <AzCore/Math/Vector2.h>
#include <AzCore/Math/Vector4.h>
#include <AzCore/Math/Color.h>

#include "AzCore/Memory/Memory.h"
#include "AzCore/Memory/SystemAllocator.h"

namespace Particle2d
{
    class Particle2DItem;

    class Particle2DUnit
    {
    public:
		AZ_CLASS_ALLOCATOR(Particle2DUnit, AZ::SystemAllocator, 0);

        void Setup(Particle2DItem* item);

        void Discard();

        void OnUpdate();

        void UpdateTimeInfo();

        void UpdateScale();

        void UpdateRotation();

        void UpdatePosition();

        void UpdateVertexColor();

        void UpdateVertexXY();

        void UpdateVertexUV(float deltaTime);

        void UpdateVertexUV(int vertexIndex);

        void CalculateVertexData(int vertexBufferStartIndex,float deltaTime);

        static AZ::Vector2 VertexXYValue[4];

        static AZ::Vector2 VertexUVValue[4];

        AZ::Vector2 LauncherPartGetUnitPosition(float checkTime);

        Particle2DItem* Item;

        AZ::Vector2 OriginPos{0,0};

        float Angle;

        AZ::Vector2 OriginMoveDir;

        bool UseForce;

        AZ::Vector2 ForceVec;

        float InResistanceMaxMoveTime = 0.0f;

        AZ::Vector2 FinalVecInResistance;

        float OriginSpeed = 0.0f;

        float OriginAngle = 0.0f;

        float AddAngle = 0.0f;

        float OriginScale = 1.0f;

        AZ::Color OriginColor;

        int OriginFrameIndex = 0;

        float SelfLiveTime;

        float SelfLifeProgress;

        AZ::Vector2 CurPos;

        int NowSpeedCurveStageIndex = -1;

        float NowSpeedCurveStageAcceleration;

        float SpeedCurveHasMoved = 0;

        bool RotationChanged = true;

        bool ScaleChanged = true;

        AZ::Vector4 CurVertexDataArr[4]{
            AZ::Vector4(0,0,0,1),
            AZ::Vector4(0,0,1,1),
            AZ::Vector4(0,0,1,0),
            AZ::Vector4(0,0,0,0)
        };

        int CurFrameColumnIndex{0};

        int CurFrameRowIndex{0};

        bool NeedUpdateFrameUV = true;

        int CurFrameIndex = -1;

        float AngleCosValue = 0.0f;

        float AngleSinValue = 0.0f;

        float CurScale = 1.0f;

        float CurAngle = 0.0f;

        int NowColorCurveStageIndex = 0;

        AZ::Color CurColor;

        AZ::Vector2 SelfOffsetArr[4];

    };

  
    
}
