#include "Renderer/Component/CameraComponent.h"

#include "Window/EBus/WindowSystemComponentBus.h"

#include "Level/EBus/LevelComponentBus.h"

#include "Renderer/EBus/RendererSystemComponentBus.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace Module
{
    void CameraComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<CameraComponent, AZ::Component>()
                ->Field("ClearFlags", &CameraComponent::m_clearFlags)
                ->Field("ClearColor", &CameraComponent::m_clearColor)
                ->Field("IsOrthographic", &CameraComponent::m_isOrthographic)
                ->Field("Size", &CameraComponent::m_size)
                ->Field("FOV", &CameraComponent::m_fov)
                ->Field("ClipNear", &CameraComponent::m_clipNear)
                ->Field("ClipFar", &CameraComponent::m_clipFar)
                ->Field("Depth", &CameraComponent::m_depth)
                ;
        }
        
        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->EBus<CameraRequestBus>("CameraRequestBus")
                ->Attribute(AZ::Script::Attributes::DisallowBroadcast, true)
                ->Event("GetClearFlags", &CameraRequestBus::Events::GetClearFlags)
                ->Event("SetClearFlags", &CameraRequestBus::Events::SetClearFlags)
                ->Event("GetClearColor", &CameraRequestBus::Events::GetClearColor)
                ->Event("SetClearColor", &CameraRequestBus::Events::SetClearColor)
                ->Event("IsOrthographic", &CameraRequestBus::Events::IsOrthographic)
                ->Event("SetOrthographic", &CameraRequestBus::Events::SetOrthographic)
                ->Event("GetOrthographicSize", &CameraRequestBus::Events::GetOrthographicSize)
                ->Event("SetOrthographicSize", &CameraRequestBus::Events::SetOrthographicSize)
                ->Event("GetFieldOfView", &CameraRequestBus::Events::GetFieldOfView)
                ->Event("SetFieldOfView", &CameraRequestBus::Events::SetFieldOfView)
                ->Event("GetClipNear", &CameraRequestBus::Events::GetClipNear)
                ->Event("SetClipNear", &CameraRequestBus::Events::SetClipNear)
                ->Event("GetClipFar", &CameraRequestBus::Events::GetClipFar)
                ->Event("SetClipFar", &CameraRequestBus::Events::SetClipFar)
                ->Event("GetRect", &CameraRequestBus::Events::GetRect)
                ->Event("SetRect", &CameraRequestBus::Events::SetRect)
                ->Event("GetDepth", &CameraRequestBus::Events::GetDepth)
                ->Event("SetDepth", &CameraRequestBus::Events::SetDepth)
                ;

            behaviorContext->Class<CameraClearFlags>("CameraClearFlags")
                ->Enum<static_cast<int>(CameraClearFlags::Skybox)>("Skybox")
                ->Enum<static_cast<int>(CameraClearFlags::SolidColor)>("SolidColor")
                ->Enum<static_cast<int>(CameraClearFlags::Depth)>("Depth")
                ->Enum<static_cast<int>(CameraClearFlags::Nothing)>("Nothing")
                ;

            behaviorContext->Class<CameraComponent>("CameraComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("clearFlags",
                    [](CameraComponent* component) { return reinterpret_cast<int&>(component->m_clearFlags); },
                    [](CameraComponent* component, const int& value) { component->m_clearFlags = static_cast<CameraClearFlags>(value); })
                ->Property("clearColor", BehaviorValueProperty(&CameraComponent::m_clearColor))
                ->Property("isOrthographic", BehaviorValueProperty(&CameraComponent::m_isOrthographic))
                ->Property("depth", BehaviorValueProperty(&CameraComponent::m_depth))
                ->Property("size", BehaviorValueProperty(&CameraComponent::m_size))
                ->Property("fov", BehaviorValueProperty(&CameraComponent::m_fov))
                ->Property("clipNear", BehaviorValueProperty(&CameraComponent::m_clipNear))
                ->Property("clipFar", BehaviorValueProperty(&CameraComponent::m_clipFar))
                ->Property("viewport", BehaviorValueProperty(&CameraComponent::m_viewport))
                ;
        }
    }

    void CameraComponent::Activate()
    {
        AZ::TransformNotificationBus::Handler::BusConnect(GetEntityId());
        CameraRequestBus::Handler::BusConnect(GetEntityId());
        AZ::Transform world = AZ::Transform::CreateIdentity();
        EBUS_EVENT_ID_RESULT(world, GetEntityId(), AZ::TransformBus, GetWorldTM);
        ResetViewMatrix(world);
    }

    void CameraComponent::Deactivate()
    {
        CameraRequestBus::Handler::BusDisconnect();
        AZ::TransformNotificationBus::Handler::BusDisconnect();
    }

    void CameraComponent::OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world)
    {
        ResetViewMatrix(world);
    }

    void CameraComponent::ResetViewMatrix(const AZ::Transform& world)
    {
        float eye[3], at[3], up[3];
        world.GetPosition().StoreToFloat3(eye);
        (world.GetBasisZ() + world.GetPosition()).StoreToFloat3(at);
        world.GetBasisY().StoreToFloat3(up);
        bx::mtxLookAt(m_modelTM, eye, at, up);
    }

    void CameraComponent::ResetView(bgfx::ViewId id)
    {
        bgfx::resetView(id);

        uint16_t clearflag = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH;
        switch (m_clearFlags)
        {
        case CameraClearFlags::Skybox:
            clearflag = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL;
            break;
        case CameraClearFlags::SolidColor:
            clearflag = BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL;
            break;
        case CameraClearFlags::Depth:
            clearflag = BGFX_CLEAR_DEPTH | BGFX_CLEAR_STENCIL;
            break;
        case CameraClearFlags::Nothing:
            clearflag = BGFX_CLEAR_NONE;
            break;
        }

        const auto color = (m_clearColor.GetR8() << 24) | (m_clearColor.GetG8() << 16) | (m_clearColor.GetB8() << 8) | m_clearColor.GetA8();
        bgfx::setViewClear(id
            , clearflag
            , color
            , 1.0f
            , 0
        );

        int width, height;
        EBUS_EVENT(WindowSystemRequestBus, GetWindowSize, width, height);

        bgfx::setViewRect(id, 0, 0, width, height);

        const float aspect = float(width) / float(height);
        float projectionMatrix[16];

        if (m_isOrthographic)
        {
            bx::mtxOrtho(projectionMatrix,
                -m_size * aspect,
                m_size * aspect,
                -m_size,
                m_size,
                m_clipNear,
                m_clipFar,
                0.0f,
                bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewMode(id, bgfx::ViewMode::Sequential);
        }
        else
        {
            bx::mtxProj(projectionMatrix, m_fov, aspect, m_clipNear, m_clipFar, bgfx::getCaps()->homogeneousDepth);
        }

        bgfx::setViewTransform(id, m_modelTM, projectionMatrix);

        bgfx::touch(id);
    }

    void CameraComponent::DrawSkybox(bgfx::ViewId id)
    {
        if (m_clearFlags == CameraClearFlags::Skybox)
        {
            // TODO
        }
    }
}

namespace AZ
{
    AZ_TYPE_INFO_SPECIALIZE(Module::CameraClearFlags, "{31519C73-C652-4B96-83CD-E1F463B49158}");
}
