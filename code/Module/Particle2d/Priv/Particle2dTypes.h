#pragma once

#include "AzCore/Math/Vector2.h"
#include "AzCore/Math/Color.h"
#include "AzCore/std/string/string.h"
#include "AzCore/std/containers/vector.h"

namespace Particle2d
{
    enum Particle2DType
    {
        Circlue,
        Cone,
        Square,
        Point,
        Circule2Center,
    };

    struct VertexData {
        float x, y, u, v;
        float r, g, b, a;
    };

    struct FrameFloatPair
    {
        float Key;
        float Value;
    };

    struct FrameColorPair
    {
        float Key;
        AZ::Color Color;
    };

    struct Particle2DLauncherData
    {

        AZ::Vector2 Offset;

        float DelayTime;
        int MaxParticlesNum = 100;
        int LayerIndex;
        int RateOverTime = 10;
        float PerUnitTime = 0.1f;
        float Duration = 10.0f;
        bool Loop;

        float UnitMinLifetime;
        float UnitMaxLifeTime = 10;
        bool UnitLifeTimeRandom;
        float UnitLifeTime = 10;

        int ParticleLauncherType;
        float LauncherShootAngle;
        float LauncherShootRandomNum = 15.0f;
        float LauncherShootLength = 1;
        float LauncherShootDepth = 1;
        bool CirculeNotRandomDir;

        bool UseSpeedCurve;
        AZStd::vector<FrameFloatPair*> SpeedKeyFrameArray;
        float UnitMinSpeed = 3;
        float UnitMaxSpeed = 5;
        float UnitSpeed = 5;
        bool UnitSpeedRandom;
        bool useForce;
        float ForceDir;
        float ForceValue;
        float Resistance;

        AZ::Color UnitMinColor;
        AZ::Color UnitMaxColor;
        bool UnitColorRandom;
        AZ::Color UnitColor;
        bool UseColorCurve;
        AZStd::vector<FrameColorPair*> ColorKeyFrameArray;

        bool UseRotation;
        float UnitRotation;
        float UnitMinRotation;
        float UnitMaxRotation;
        bool UnitRotationRandom;
        bool UseAddRotation;
        float UnitAddRotation;
        float UnitMinAddRotation;
        float UnitMaxAddRotation;
        bool UnitAddRotationRandom;

        float UnitMinSize;
        float UnitMaxSize = 1;
        bool UnitSizeRandom;
        float UnitSizeX = 100.0f;
        float UnitSizeY = 100.0f;
        bool UseLifeTimeSizeChange;
        float UnitLifetimeStartSize = 1;
        float UnitLifetimeFinSize = 1;

        int UnitFrameColumnNum = 1;
        int UnitFrameRowNum = 1;
        int UnitFrameFPS;
        bool UnitFrameRandom;
        bool UseFrame;

        bool UseUVScroll;
        float XScrollSpeed;
        float YScrollSpeed;

        AZStd::string ShaderType;
        AZStd::string ImageName;
    };

    struct Particle2DStructData {
        int Index;
        AZStd::vector<int> ChildIndexArray;
    };

    struct Particle2DSystemDesc
    {
        AZStd::vector<Particle2DStructData*> LauncherStruct;
        AZStd::vector<Particle2DLauncherData*> LauncherData;
    };

    enum RenderType
    {
        Blend = 10,
        Add
    };

    const float angleToRaidNum = 180.0f / 3.14159f;
}
