/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#ifndef AZ_UNITY_BUILD

#include <AzCore/Component/TransformComponent.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Component/Entity.h>
#include <AzCore/Component/ComponentApplicationBus.h>
#include <AzCore/Math/MathFunc.h>

namespace AZ
{
    class BehaviorTransformNotificationBusHandler : public TransformNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(BehaviorTransformNotificationBusHandler, "{9CEF4DAB-F359-4A3E-9856-7780281E0DAA}", AZ::SystemAllocator
            , OnTransformChanged
            , OnParentChanged
            , OnChildAdded
            , OnChildRemoved
        );

        void OnTransformChanged(const Transform& localTM, const Transform& worldTM) override
        {
            Call(FN_OnTransformChanged, localTM, worldTM);
        }

        void OnParentChanged(EntityId oldParent, EntityId newParent) override
        {
            Call(FN_OnParentChanged, oldParent, newParent);
        }

        void OnChildAdded(EntityId child) override
        {
            Call(FN_OnChildAdded, child);
        }

        void OnChildRemoved(EntityId child) override
        {
            Call(FN_OnChildRemoved, child);
        }
    };

    void TransformConfigConstructor(TransformConfig* self, ScriptDataContext& dc)
    {
        const int numArgs = dc.GetNumArguments();
        if (numArgs == 0)
        {
            new(self) TransformConfig();
        }
        else if (numArgs == 1 && dc.IsClass<Transform>(0))
        {
            Transform transform;
            dc.ReadArg(0, transform);

            new(self) TransformConfig(transform);
        }
        else
        {
            dc.GetScriptContext()->Error(AZ::ScriptContext::ErrorType::Error, true, "Invalid constructor call. Valid overrides are: TransformConfig(), TransfomConfig(Transform)");

            new(self) TransformConfig();
        }
    }

} // namespace AZ

namespace AZ
{
    //=========================================================================
    // TransformComponent
    // [8/9/2013]
    //=========================================================================
    TransformComponent::TransformComponent()
        : m_parentTM(nullptr)
        , m_onNewParentKeepWorldTM(true)
        , m_parentActivationTransformMode(ParentActivationTransformMode::MaintainOriginalRelativeTransform)
        , m_isStatic(false)
    {
        m_localTM = AZ::Transform::CreateIdentity();
        m_worldTM = AZ::Transform::CreateIdentity();
    }

    TransformComponent::TransformComponent(const TransformComponent& copy)
        : m_localTM(copy.m_localTM)
        , m_worldTM(copy.m_worldTM)
        , m_parentId(copy.m_parentId)
        , m_parentTM(copy.m_parentTM)
        , m_notificationBus(nullptr)
        , m_onNewParentKeepWorldTM(copy.m_onNewParentKeepWorldTM)
        , m_parentActivationTransformMode(copy.m_parentActivationTransformMode)
        , m_isStatic(copy.m_isStatic)
    {
    }

    TransformComponent::~TransformComponent()
    {
    }

    bool TransformComponent::ReadInConfig(const AZ::ComponentConfig* baseConfig)
    {
        if (auto config = azrtti_cast<const AZ::TransformConfig*>(baseConfig))
        {
            m_localTM = config->m_localTransform;
            m_worldTM = config->m_worldTransform;
            m_parentId = config->m_parentId;
            m_parentActivationTransformMode = config->m_parentActivationTransformMode;
            m_isStatic = config->m_isStatic;
            return true;
        }
        return false;
    }

    bool TransformComponent::WriteOutConfig(AZ::ComponentConfig* baseConfig) const
    {
        if (auto config = azrtti_cast<AZ::TransformConfig*>(baseConfig))
        {
            config->m_localTransform = m_localTM;
            config->m_worldTransform = m_worldTM;
            config->m_parentId = m_parentId;
            config->m_parentActivationTransformMode = m_parentActivationTransformMode;
            config->m_isStatic = m_isStatic;
            return true;
        }
        return false;
    }

    void TransformComponent::Activate()
    {
        AZ::TransformBus::Handler::BusConnect(m_entity->GetId());
        AZ::TransformNotificationBus::Bind(m_notificationBus, m_entity->GetId());

        const bool keepWorldTm = (m_parentActivationTransformMode == ParentActivationTransformMode::MaintainCurrentWorldTransform || !m_parentId.IsValid());
        SetParentImpl(m_parentId, keepWorldTm);
    }

    void TransformComponent::Deactivate()
    {
        EBUS_EVENT_ID(m_parentId, AZ::TransformNotificationBus, OnChildRemoved, GetEntityId());

        m_notificationBus = nullptr;
        if (m_parentId.IsValid())
        {
            AZ::TransformNotificationBus::Handler::BusDisconnect();
            AZ::TransformHierarchyInformationBus::Handler::BusDisconnect();
            AZ::EntityBus::Handler::BusDisconnect();
        }
        AZ::TransformBus::Handler::BusDisconnect();
    }

    void TransformComponent::SetLocalTM(const AZ::Transform& tm)
    {
        if (AreMoveRequestsAllowed())
        {
            SetLocalTMImpl(tm);
        }
    }

    void TransformComponent::SetWorldTM(const AZ::Transform& tm)
    {
        if (AreMoveRequestsAllowed())
        {
            SetWorldTMImpl(tm);
        }
    }

    void TransformComponent::SetParent(AZ::EntityId id)
    {
        SetParentImpl(id, true);
    }

    void TransformComponent::SetParentRelative(AZ::EntityId id)
    {
        SetParentImpl(id, m_isStatic);
    }

    void TransformComponent::SetWorldTranslation(const AZ::Vector3& newPosition)
    {
        AZ::Transform newWorldTransform = m_worldTM;
        newWorldTransform.SetTranslation(newPosition);
        SetWorldTM(newWorldTransform);
    }

    void TransformComponent::SetLocalTranslation(const AZ::Vector3& newPosition)
    {
        AZ::Transform newLocalTransform = m_localTM;
        newLocalTransform.SetTranslation(newPosition);
        SetLocalTM(newLocalTransform);
    }

    AZ::Vector3 TransformComponent::GetWorldTranslation()
    {
        return m_worldTM.GetPosition();
    }

    AZ::Vector3 TransformComponent::GetLocalTranslation()
    {
        return m_localTM.GetPosition();
    }

    void TransformComponent::MoveEntity(const AZ::Vector3& offset)
    {
        const AZ::Vector3& worldPosition = m_worldTM.GetPosition();
        SetWorldTranslation(worldPosition + offset);
    }

    void TransformComponent::SetWorldX(float x)
    {
        const AZ::Vector3& worldPosition = m_worldTM.GetPosition();
        SetWorldTranslation(AZ::Vector3(x, worldPosition.GetY(), worldPosition.GetZ()));
    }

    void TransformComponent::SetWorldY(float y)
    {
        const AZ::Vector3& worldPosition = m_worldTM.GetPosition();
        SetWorldTranslation(AZ::Vector3(worldPosition.GetX(), y, worldPosition.GetZ()));
    }

    void TransformComponent::SetWorldZ(float z)
    {
        const AZ::Vector3& worldPosition = m_worldTM.GetPosition();
        SetWorldTranslation(AZ::Vector3(worldPosition.GetX(), worldPosition.GetY(), z));
    }

    float TransformComponent::GetWorldX()
    {
        return GetWorldTranslation().GetX();
    }

    float TransformComponent::GetWorldY()
    {
        return GetWorldTranslation().GetY();
    }

    float TransformComponent::GetWorldZ()
    {
        return GetWorldTranslation().GetZ();
    }

    void TransformComponent::SetLocalX(float x)
    {
        AZ::Vector3 newLocalTranslation = m_localTM.GetTranslation();
        newLocalTranslation.SetX(x);
        SetLocalTranslation(newLocalTranslation);
    }

    void TransformComponent::SetLocalY(float y)
    {
        AZ::Vector3 newLocalTranslation = m_localTM.GetTranslation();
        newLocalTranslation.SetY(y);
        SetLocalTranslation(newLocalTranslation);
    }

    void TransformComponent::SetLocalZ(float z)
    {
        AZ::Vector3 newLocalTranslation = m_localTM.GetTranslation();
        newLocalTranslation.SetZ(z);
        SetLocalTranslation(newLocalTranslation);
    }

    float TransformComponent::GetLocalX()
    {
        float localX = m_localTM.GetTranslation().GetX();
        return localX;
    }

    float TransformComponent::GetLocalY()
    {
        float localY = m_localTM.GetTranslation().GetY();
        return localY;
    }

    float TransformComponent::GetLocalZ()
    {
        float localZ = m_localTM.GetTranslation().GetZ();
        return localZ;
    }

    AZ::Vector3 TransformComponent::GetWorldRotation()
    {
        AZ::Transform rotate = m_worldTM;
        rotate.ExtractScaleExact();
        AZ::Vector3 angles = AZ::ConvertTransformToEulerRadians(rotate);
        return angles;
    }

    AZ::Quaternion TransformComponent::GetWorldRotationQuaternion()
    {
        AZ::Transform rotate = m_worldTM;
        rotate.ExtractScaleExact();
        AZ::Quaternion quat = AZ::Quaternion::CreateFromTransform(rotate);
        return quat;
    }

    void TransformComponent::SetLocalRotation(const AZ::Vector3& eulerRadianAngles)
    {
        AZ::Transform newLocalTM = AZ::ConvertEulerRadiansToTransformPrecise(eulerRadianAngles);
        AZ::Vector3 scale = m_localTM.RetrieveScaleExact();
        newLocalTM.MultiplyByScale(scale);
        newLocalTM.SetTranslation(m_localTM.GetTranslation());
        SetLocalTM(newLocalTM);
    }

    void TransformComponent::SetLocalRotationQuaternion(const AZ::Quaternion& quaternion)
    {
        AZ::Transform newLocalTM = AZ::Transform::CreateFromQuaternion(quaternion);
        AZ::Vector3 scale = m_localTM.RetrieveScaleExact();
        newLocalTM.MultiplyByScale(scale);
        newLocalTM.SetTranslation(m_localTM.GetTranslation());
        SetLocalTM(newLocalTM);
    }

    void TransformComponent::RotateAroundLocalX(float eulerAngleRadian)
    {
        AZ::Transform currentLocalTM = m_localTM;
        AZ::Vector3 xAxis = currentLocalTM.GetBasisX();
        AZ::Quaternion xRotate = AZ::Quaternion::CreateFromAxisAngle(xAxis, eulerAngleRadian);

        AZ::Vector3 translation = currentLocalTM.GetTranslation();
        AZ::Vector3 scale = currentLocalTM.ExtractScaleExact();

        AZ::Quaternion currentRotate = AZ::Quaternion::CreateFromTransform(currentLocalTM);
        AZ::Quaternion newRotate = xRotate * currentRotate;
        newRotate.NormalizeExact();

        AZ::Transform newLocalTM = AZ::Transform::CreateFromQuaternion(newRotate);
        newLocalTM.MultiplyByScale(scale);
        newLocalTM.SetTranslation(translation);

        SetLocalTM(newLocalTM);
    }

    void TransformComponent::RotateAroundLocalY(float eulerAngleRadian)
    {
        AZ::Transform currentLocalTM = m_localTM;
        AZ::Vector3 yAxis = currentLocalTM.GetBasisY();
        AZ::Quaternion yRotate = AZ::Quaternion::CreateFromAxisAngle(yAxis, eulerAngleRadian);

        AZ::Vector3 translation = currentLocalTM.GetTranslation();
        AZ::Vector3 scale = currentLocalTM.ExtractScaleExact();

        AZ::Quaternion currentRotate = AZ::Quaternion::CreateFromTransform(currentLocalTM);
        AZ::Quaternion newRotate = yRotate * currentRotate;
        newRotate.NormalizeExact();

        AZ::Transform newLocalTM = AZ::Transform::CreateFromQuaternion(newRotate);
        newLocalTM.MultiplyByScale(scale);
        newLocalTM.SetTranslation(translation);

        SetLocalTM(newLocalTM);
    }

    void TransformComponent::RotateAroundLocalZ(float eulerAngleRadian)
    {
        AZ::Transform currentLocalTM = m_localTM;
        AZ::Vector3 zAxis = currentLocalTM.GetBasisZ();
        AZ::Quaternion zRotate = AZ::Quaternion::CreateFromAxisAngle(zAxis, eulerAngleRadian);

        AZ::Vector3 translation = currentLocalTM.GetTranslation();
        AZ::Vector3 scale = currentLocalTM.ExtractScaleExact();

        AZ::Quaternion currentRotate = AZ::Quaternion::CreateFromTransform(currentLocalTM);
        AZ::Quaternion newRotate = zRotate * currentRotate;
        newRotate.NormalizeExact();

        AZ::Transform newLocalTM = AZ::Transform::CreateFromQuaternion(newRotate);
        newLocalTM.MultiplyByScale(scale);
        newLocalTM.SetTranslation(translation);

        SetLocalTM(newLocalTM);
    }

    AZ::Vector3 TransformComponent::GetLocalRotation()
    {
        AZ::Transform rotate = m_localTM;
        rotate.ExtractScaleExact();
        AZ::Vector3 angles = AZ::ConvertTransformToEulerRadians(rotate);
        return angles;
    }

    AZ::Quaternion TransformComponent::GetLocalRotationQuaternion()
    {
        AZ::Transform rotate = m_localTM;
        rotate.ExtractScaleExact();
        AZ::Quaternion quat = AZ::Quaternion::CreateFromTransform(rotate);
        return quat;
    }

    void TransformComponent::SetLocalScale(const AZ::Vector3& scale)
    {
        AZ::Transform newLocalTM = m_localTM;
        newLocalTM.ExtractScaleExact();
        newLocalTM.MultiplyByScale(scale);
        SetLocalTM(newLocalTM);
    }

    void TransformComponent::SetLocalScaleX(float scaleX)
    {
        AZ::Transform newLocalTM = m_localTM;
        AZ::Vector3 newScale = newLocalTM.ExtractScaleExact();
        newScale.SetX(scaleX);
        newLocalTM.MultiplyByScale(newScale);
        SetLocalTM(newLocalTM);
    }

    void TransformComponent::SetLocalScaleY(float scaleY)
    {
        AZ::Transform newLocalTM = m_localTM;
        AZ::Vector3 newScale = newLocalTM.ExtractScaleExact();
        newScale.SetY(scaleY);
        newLocalTM.MultiplyByScale(newScale);
        SetLocalTM(newLocalTM);
    }

    void TransformComponent::SetLocalScaleZ(float scaleZ)
    {
        AZ::Transform newLocalTM = m_localTM;
        AZ::Vector3 newScale = newLocalTM.ExtractScaleExact();
        newScale.SetZ(scaleZ);
        newLocalTM.MultiplyByScale(newScale);
        SetLocalTM(newLocalTM);
    }

    AZ::Vector3 TransformComponent::GetLocalScale()
    {
        AZ::Vector3 scale = m_localTM.RetrieveScaleExact();
        return scale;
    }

    AZ::Vector3 TransformComponent::GetWorldScale()
    {
        AZ::Vector3 scale = m_worldTM.RetrieveScaleExact();
        return scale;
    }

    AZStd::vector<AZ::EntityId> TransformComponent::GetChildren()
    {
        AZStd::vector<AZ::EntityId> children;
        EBUS_EVENT_ID(GetEntityId(), AZ::TransformHierarchyInformationBus, GatherChildren, children);
        return children;
    }

    AZStd::vector<AZ::EntityId> TransformComponent::GetAllDescendants()
    {
        AZStd::vector<AZ::EntityId> descendants = GetChildren();
        for (size_t i = 0; i < descendants.size(); ++i)
        {
            EBUS_EVENT_ID(descendants[i], AZ::TransformHierarchyInformationBus, GatherChildren, descendants);
        }
        return descendants;
    }

    AZStd::vector<AZ::EntityId> TransformComponent::GetEntityAndAllDescendants()
    {
        AZStd::vector<AZ::EntityId> descendants = { GetEntityId() };
        for (size_t i = 0; i < descendants.size(); ++i)
        {
            EBUS_EVENT_ID(descendants[i], AZ::TransformHierarchyInformationBus, GatherChildren, descendants);
        }
        return descendants;
    }

    bool TransformComponent::IsStaticTransform()
    {
        return m_isStatic;
    }

    void TransformComponent::OnTransformChanged(const AZ::Transform& parentLocalTM, const AZ::Transform& parentWorldTM)
    {
        OnTransformChangedImpl(parentLocalTM, parentWorldTM);
    }

    void TransformComponent::GatherChildren(AZStd::vector<AZ::EntityId>& children)
    {
        children.push_back(GetEntityId());
    }

    void TransformComponent::OnEntityActivated(const AZ::EntityId& parentEntityId)
    {
        OnEntityActivatedImpl(parentEntityId);
    }

    void TransformComponent::OnEntityDeactivated(const AZ::EntityId& parentEntityId)
    {
        OnEntityDeactivateImpl(parentEntityId);
    }

    void TransformComponent::SetParentImpl(AZ::EntityId parentId, bool isKeepWorldTM)
    {
        AZ::EntityId oldParent = m_parentId;
        if (m_parentId.IsValid())
        {
            AZ::TransformNotificationBus::Handler::BusDisconnect();
            AZ::TransformHierarchyInformationBus::Handler::BusDisconnect();
            AZ::EntityBus::Handler::BusDisconnect();
        }

        m_parentId = parentId;
        if (m_parentId.IsValid())
        {
            m_onNewParentKeepWorldTM = isKeepWorldTM;

            AZ::TransformNotificationBus::Handler::BusConnect(m_parentId);
            AZ::TransformHierarchyInformationBus::Handler::BusConnect(m_parentId);
            AZ::EntityBus::Handler::BusConnect(m_parentId);
        }
        else
        {
            m_parentTM = nullptr;

            if (isKeepWorldTM)
            {
                SetWorldTM(m_worldTM);
            }
            else
            {
                SetLocalTM(m_localTM);
            }

            if (oldParent.IsValid())
            {
                EBUS_EVENT_PTR(m_notificationBus, AZ::TransformNotificationBus, OnTransformChanged, m_localTM, m_worldTM);
            }
        }

        EBUS_EVENT_PTR(m_notificationBus, AZ::TransformNotificationBus, OnParentChanged, oldParent, parentId);

        if (oldParent != parentId) // Don't send removal notification while activating.
        {
            EBUS_EVENT_ID(oldParent, AZ::TransformNotificationBus, OnChildRemoved, GetEntityId());
        }

        EBUS_EVENT_ID(parentId, AZ::TransformNotificationBus, OnChildAdded, GetEntityId());
    }

    void TransformComponent::SetLocalTMImpl(const AZ::Transform& tm)
    {
        m_localTM = tm;
        ComputeWorldTM();  // We can user dirty flags and compute it later on demand
    }

    void TransformComponent::SetWorldTMImpl(const AZ::Transform& tm)
    {
        m_worldTM = tm;
        ComputeLocalTM(); // We can user dirty flags and compute it later on demand
    }

    void TransformComponent::OnTransformChangedImpl(const AZ::Transform& /*parentLocalTM*/, const AZ::Transform& parentWorldTM)
    {
        // Called when our parent transform changes
        // Ignore the event until we've already derived our local transform.
        if (m_parentTM)
        {
            m_worldTM = parentWorldTM * m_localTM;
            EBUS_EVENT_PTR(m_notificationBus, AZ::TransformNotificationBus, OnTransformChanged, m_localTM, m_worldTM);
        }
    }

    void TransformComponent::OnEntityActivatedImpl(const AZ::EntityId& parentEntityId)
    {
        AZ_Assert(parentEntityId == m_parentId, "We expect to receive notifications only from the current parent!");

        AZ::Entity* parentEntity = nullptr;
        EBUS_EVENT_RESULT(parentEntity, AZ::ComponentApplicationBus, FindEntity, parentEntityId);
        AZ_Assert(parentEntity, "We expect to have a parent entity associated with the provided parent's entity Id.");
        if (parentEntity)
        {
            m_parentTM = parentEntity->FindComponent<TransformComponent>();

            AZ_Warning("TransformComponent", !m_isStatic || m_parentTM->IsStaticTransform(),
                "Entity '%s' %s has static transform, but parent has non-static transform. This may lead to unexpected movement.",
                GetEntity()->GetName().c_str(), GetEntityId().ToString().c_str());

            if (m_onNewParentKeepWorldTM)
            {
                ComputeLocalTM();
            }
            else
            {
                ComputeWorldTM();
            }
        }
    }

    void TransformComponent::OnEntityDeactivateImpl(const AZ::EntityId& parentEntityId)
    {
        (void)parentEntityId;
        AZ_Assert(parentEntityId == m_parentId, "We expect to receive notifications only from the current parent!");
        m_parentTM = nullptr;
        ComputeLocalTM();
    }

    void TransformComponent::ComputeLocalTM()
    {
        if (m_parentTM)
        {
            m_localTM = m_parentTM->GetWorldTM().GetInverseFull() * m_worldTM;
        }
        else
        {
            m_localTM = m_worldTM;
        }
        EBUS_EVENT_PTR(m_notificationBus, AZ::TransformNotificationBus, OnTransformChanged, m_localTM, m_worldTM);
    }

    void TransformComponent::ComputeWorldTM()
    {
        if (m_parentTM)
        {
            m_worldTM = m_parentTM->GetWorldTM() * m_localTM;
        }
        else if (!m_parentId.IsValid())
        {
            m_worldTM = m_localTM;
        }

        EBUS_EVENT_PTR(m_notificationBus, AZ::TransformNotificationBus, OnTransformChanged, m_localTM, m_worldTM);
    }

    bool TransformComponent::AreMoveRequestsAllowed() const
    {
        // Don't allow static transform to be moved while entity is activated.
        // But do allow a static transform to be moved when the entity is deactivated.
        if (m_isStatic && m_entity && (m_entity->GetState() > AZ::Entity::ES_INIT))
        {
            return false;
        }

        return true;
    }

    void TransformComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("TransformService", 0x8ee22c50));
    }

    void TransformComponent::Reflect(AZ::ReflectContext* reflection)
    {
        AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection);
        if (serializeContext)
        {
            serializeContext->Class<TransformComponent, AZ::Component>()
                ->Field("Parent", &TransformComponent::m_parentId)
                ->Field("Transform", &TransformComponent::m_worldTM)
                ->Field("LocalTransform", &TransformComponent::m_localTM)
                ->Field("ParentActivationTransformMode", &TransformComponent::m_parentActivationTransformMode)
                ->Field("IsStatic", &TransformComponent::m_isStatic)
                ;
        }

        AZ::BehaviorContext* behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection);
        if(behaviorContext)
        {
            behaviorContext->EBus<AZ::TransformNotificationBus>("TransformNotificationBus")->
                Handler<AZ::BehaviorTransformNotificationBusHandler>();

            behaviorContext->Class<TransformComponent>("TransformComponent")
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Constructor()
                ->Property("m_localTM", BehaviorValueProperty(&TransformComponent::m_localTM))
                ->Property("m_worldTM", BehaviorValueProperty(&TransformComponent::m_worldTM))
                ->Property("m_parentId", BehaviorValueProperty(&TransformComponent::m_parentId))
                ->Property("m_parentActivationTransformMode",
                    [](AZ::TransformComponent* component) { return (int&)(component->m_parentActivationTransformMode); },
                    [](AZ::TransformComponent* component, const int& i) { component->m_parentActivationTransformMode = (AZ::TransformComponent::ParentActivationTransformMode)i; })
                ->Property("m_isStatic", BehaviorValueProperty(&TransformComponent::m_isStatic))
                ->Property("m_onNewParentKeepWorldTM", BehaviorValueProperty(&TransformComponent::m_onNewParentKeepWorldTM))
                ->RequestBus("TransformBus")
                ;

            behaviorContext->EBus<AZ::TransformBus>("TransformBus")
                ->Attribute(AZ::Script::Attributes::DisallowBroadcast, true)
                ->Event("GetLocalTM", &AZ::TransformBus::Events::GetLocalTM)
                ->Event("GetWorldTM", &AZ::TransformBus::Events::GetWorldTM)
                ->Event("GetParentId", &AZ::TransformBus::Events::GetParentId)
                ->Event("GetLocalAndWorld", &AZ::TransformBus::Events::GetLocalAndWorld)
                    ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::All)
                ->Event("SetLocalTM", &AZ::TransformBus::Events::SetLocalTM)
                ->Event("SetWorldTM", &AZ::TransformBus::Events::SetWorldTM)
                ->Event("SetParent", &AZ::TransformBus::Events::SetParent)
                ->Event("SetParentRelative", &AZ::TransformBus::Events::SetParentRelative)
                ->Event("SetWorldTranslation", &AZ::TransformBus::Events::SetWorldTranslation)
                ->Event("SetLocalTranslation", &AZ::TransformBus::Events::SetLocalTranslation)
                ->Event("GetWorldTranslation", &AZ::TransformBus::Events::GetWorldTranslation)
                ->Event("GetLocalTranslation", &AZ::TransformBus::Events::GetLocalTranslation)
                ->VirtualProperty("Position", "GetLocalTranslation", "SetLocalTranslation")
                ->Event("MoveEntity", &AZ::TransformBus::Events::MoveEntity)
                ->Event("SetWorldX", &AZ::TransformBus::Events::SetWorldX)
                ->Event("SetWorldY", &AZ::TransformBus::Events::SetWorldY)
                ->Event("SetWorldZ", &AZ::TransformBus::Events::SetWorldZ)
                ->Event("GetWorldX", &AZ::TransformBus::Events::GetWorldX)
                ->Event("GetWorldY", &AZ::TransformBus::Events::GetWorldY)
                ->Event("GetWorldZ", &AZ::TransformBus::Events::GetWorldZ)
                ->Event("SetLocalX", &AZ::TransformBus::Events::SetLocalX)
                ->Event("SetLocalY", &AZ::TransformBus::Events::SetLocalY)
                ->Event("SetLocalZ", &AZ::TransformBus::Events::SetLocalZ)
                ->Event("GetLocalX", &AZ::TransformBus::Events::GetLocalX)
                ->Event("GetLocalY", &AZ::TransformBus::Events::GetLocalY)
                ->Event("GetLocalZ", &AZ::TransformBus::Events::GetLocalZ)
                ->Event("GetWorldRotation", &AZ::TransformBus::Events::GetWorldRotation)
                ->Event("GetWorldRotationQuaternion", &AZ::TransformBus::Events::GetWorldRotationQuaternion)
                ->Event("SetLocalRotation", &AZ::TransformBus::Events::SetLocalRotation)
                ->Event("SetLocalRotationQuaternion", &AZ::TransformBus::Events::SetLocalRotationQuaternion)
                ->Event("RotateAroundLocalX", &AZ::TransformBus::Events::RotateAroundLocalX)
                ->Event("RotateAroundLocalY", &AZ::TransformBus::Events::RotateAroundLocalY)
                ->Event("RotateAroundLocalZ", &AZ::TransformBus::Events::RotateAroundLocalZ)
                ->Event("GetLocalRotation", &AZ::TransformBus::Events::GetLocalRotation)
                ->Event("GetLocalRotationQuaternion", &AZ::TransformBus::Events::GetLocalRotationQuaternion)
                ->VirtualProperty("Rotation", "GetLocalRotationQuaternion", "SetLocalRotationQuaternion")
                ->Event("SetLocalScale", &AZ::TransformBus::Events::SetLocalScale)
                ->Event("SetLocalScaleX", &AZ::TransformBus::Events::SetLocalScaleX)
                ->Event("SetLocalScaleY", &AZ::TransformBus::Events::SetLocalScaleY)
                ->Event("SetLocalScaleZ", &AZ::TransformBus::Events::SetLocalScaleZ)
                ->Event("GetLocalScale", &AZ::TransformBus::Events::GetLocalScale)
                ->VirtualProperty("Scale", "GetLocalScale", "SetLocalScale")
                ->Event("GetWorldScale", &AZ::TransformBus::Events::GetWorldScale)
                ->Event("GetChildren", &AZ::TransformBus::Events::GetChildren)
                ->Event("GetAllDescendants", &AZ::TransformBus::Events::GetAllDescendants)
                ->Event("GetEntityAndAllDescendants", &AZ::TransformBus::Events::GetEntityAndAllDescendants)
                ->Event("IsStaticTransform", &AZ::TransformBus::Events::IsStaticTransform)
                ;

            behaviorContext->Class<AZ::TransformConfig>()
                ->Attribute(AZ::Script::Attributes::ExcludeFrom, AZ::Script::Attributes::ExcludeFlags::Preview)
                ->Attribute(AZ::Script::Attributes::ConstructorOverride, &AZ::TransformConfigConstructor)
                ->Enum<(int)AZ::TransformConfig::ParentActivationTransformMode::MaintainOriginalRelativeTransform>("MaintainOriginalRelativeTransform")
                ->Enum<(int)AZ::TransformConfig::ParentActivationTransformMode::MaintainCurrentWorldTransform>("MaintainCurrentWorldTransform")
                ->Constructor()
                ->Constructor<const AZ::Transform&>()
                ->Property("worldTransform", BehaviorValueProperty(&AZ::TransformConfig::m_worldTransform))
                ->Property("localTransform", BehaviorValueProperty(&AZ::TransformConfig::m_localTransform))
                ->Property("parentId", BehaviorValueProperty(&AZ::TransformConfig::m_parentId))
                ->Property("parentActivationTransformMode",
                    [](AZ::TransformConfig* config) { return (int&)(config->m_parentActivationTransformMode); },
                    [](AZ::TransformConfig* config, const int& i) { config->m_parentActivationTransformMode = (AZ::TransformConfig::ParentActivationTransformMode)i; })
                ->Property("isStatic", BehaviorValueProperty(&AZ::TransformConfig::m_isStatic))
                ;
        }
    }
} // namespace AZ

#endif  // AZ_UNITY_BUILD
