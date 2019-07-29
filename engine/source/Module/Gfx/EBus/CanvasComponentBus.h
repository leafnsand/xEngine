#pragma once

#include "AzCore/Component/ComponentBus.h"
#include "AzCore/Math/Color.h"

namespace Module
{
    class TextureAsset;

    class CanvasRequest : public AZ::ComponentBus
    {
    public:
        virtual bool IsValid() const = 0;

        virtual void DrawImage(TextureAsset* image, AZ::Vector4& rect, AZ::Matrix4x4& transform, const AZ::Color& color) = 0;

        virtual void DrawArcImage(TextureAsset* image, AZ::Vector4& rect, float x, float y, float radius, float angle1, float angle2, const AZ::Color& color) = 0;

        virtual void DrawCircleImage(TextureAsset* image, AZ::Vector4& rect, float x, float y, float radius, const AZ::Color& color) = 0;

        virtual void DrawArc(float x, float y, float radius, float angle1, float angle2, const AZ::Color& color) = 0;

        virtual void DrawCircle(float x, float y, float radius, const AZ::Color& color) = 0;

        virtual void DrawRectangle(float x, float y, float w, float h, const AZ::Color& color) = 0;

        virtual void DrawLine(float pointA_x, float pointA_y, float pointB_x, float pointB_y, float lineWidth, const AZ::Color& color) = 0;

        virtual void Clear() = 0;
    };

    using CanvasRequestBus = AZ::EBus<CanvasRequest>;
}