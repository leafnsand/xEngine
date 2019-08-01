#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/Color.h>

namespace Module
{
    enum class CameraClearFlags : AZ::u8
    {
        Skybox     = 1,
        SolidColor = 2,
        Depth      = 3,
        Nothing    = 4,
    };

    class CameraRequest : public AZ::ComponentBus
    {
    public:
        AZ_RTTI(CameraRequest, "{44DAE0D0-85CC-4A53-8E1A-C31D0C4FB254}", AZ::ComponentBus)

        virtual CameraClearFlags   GetClearFlags() const                 = 0;
        virtual void               SetClearFlags(CameraClearFlags flags) = 0;

        virtual const AZ::Color&   GetClearColor() const                 = 0;
        virtual void               SetClearColor(const AZ::Color& color) = 0;

        virtual bool               IsOrthographic() const                = 0;
        virtual void               SetOrthographic(bool value)           = 0;

        virtual float              GetOrthographicSize() const           = 0;
        virtual void               SetOrthographicSize(float size)       = 0;

        virtual float              GetFieldOfView() const                = 0;
        virtual void               SetFieldOfView(float fov)             = 0;

        virtual float              GetClipNear() const                   = 0;
        virtual void               SetClipNear(float value)              = 0;

        virtual float              GetClipFar() const                    = 0;
        virtual void               SetClipFar(float value)               = 0;

        virtual const AZ::Vector4& GetRect() const                       = 0;
        virtual void               SetRect(const AZ::Vector4& rect)      = 0;

        virtual int                GetDepth() const                      = 0;
        virtual void               SetDepth(int depth)                   = 0;

    };

    using CameraRequestBus = AZ::EBus<CameraRequest>;
}
