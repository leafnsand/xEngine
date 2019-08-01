#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>

#include "Renderer/EBus/CameraComponentBus.h"

#include <AzCore/Math/Color.h>

#include <bgfx/bgfx.h>


namespace Module
{
    class CameraComponent
        : public AZ::Component
        , protected AZ::TransformNotificationBus::Handler
        , protected CameraRequestBus::Handler
    {
    public:
        AZ_COMPONENT(CameraComponent, "{555C7683-39B5-4090-9640-459B4EF9A5AC}");

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
        {
            provided.push_back(AZ_CRC("CameraService"));
        }

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
        {
            incompatible.push_back(AZ_CRC("CameraService"));
        }

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
        {
            dependent.push_back(AZ_CRC("TransformService"));
        }

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
        {
            required.push_back(AZ_CRC("TransformService"));
        }

        static void Reflect(AZ::ReflectContext* reflection);

        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::Component
        void Activate() override;
        void Deactivate() override;
        /////////////////////////////////////////////////////////////////////////////////////

        void ResetView(bgfx::ViewId id);
        void DrawSkybox(bgfx::ViewId id);

    protected:
        /////////////////////////////////////////////////////////////////////////////////////
        // AZ::TransformNotificationBus::Handler
        void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override;
        /////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////
        // CameraRequestBus::Handler
        CameraClearFlags   GetClearFlags() const override                 { return m_clearFlags; }
        void               SetClearFlags(CameraClearFlags value) override { m_clearFlags = value; }

        const AZ::Color&   GetClearColor() const override                 { return m_clearColor; }
        void               SetClearColor(const AZ::Color& color) override { m_clearColor = color; }

        bool               IsOrthographic() const override                { return m_isOrthographic; }
        void               SetOrthographic(bool value) override           { m_isOrthographic = value; }

        float              GetOrthographicSize() const override           { return m_size; }
        void               SetOrthographicSize(float size) override       { m_size = size; }

        float              GetFieldOfView() const override                { return m_fov; }
        void               SetFieldOfView(float value) override           { m_fov = value; }

        float              GetClipNear() const override                   { return m_clipNear; }
        void               SetClipNear(float value) override              { m_clipNear = value; }

        float              GetClipFar() const override                    { return m_clipFar; }
        void               SetClipFar(float value) override               { m_clipFar = value; }

        const AZ::Vector4& GetRect() const override                       { return m_viewport; }
        void               SetRect(const AZ::Vector4& rect) override      { m_viewport = rect; }
        
        int                GetDepth() const override                      { return m_depth; }
        void               SetDepth(int value) override                   { m_depth = value;}
        /////////////////////////////////////////////////////////////////////////////////////

    private:
        void ResetViewMatrix(const AZ::Transform& world);

        CameraClearFlags m_clearFlags     = CameraClearFlags::Skybox;
        AZ::Color        m_clearColor     = AZ::Color::CreateZero();
        bool             m_isOrthographic = false;

        int              m_depth          = 0;
        float            m_size           = 5.0f;
        float            m_fov            = 60.0f;

        float            m_clipNear       = 0.3f;
        float            m_clipFar        = 1000.0f;

        AZ::Vector4      m_viewport       = AZ::Vector4(0, 0, 1, 1);

        float            m_modelTM[16]    = {};

        friend class RendererSystemComponent;
    };
}
