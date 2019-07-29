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

/** @file
 * Header file for buses that dispatch and receive events related to positioning,
 * rotating, scaling, and parenting an entity.
 */

#ifndef AZCORE_TRANSFORM_BUS_H
#define AZCORE_TRANSFORM_BUS_H

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/Math/InterpolationSample.h>
#include <AzCore/Math/Transform.h>

namespace AZ
{
    class Transform;

    /**
     * Interface for AZ::TransformBus, which is an EBus that receives requests
     * to translate (position), rotate, and scale an entity in 3D space. It
     * also receives requests to get and set the parent of an entity and get
     * the descendants of an entity.
     *
     * An entity's local transform is the entity's position relative to its
     * parent entity. An entity's world transform is the entity's position
     * within the entire game space.
     */
    class TransformInterface
        : public ComponentBus
    {
    public:
        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides - application is a singleton.
        /**
         * Overrides the default AZ::EBusTraits handler policy to allow one
         * listener only.
         */
        static const EBusHandlerPolicy HandlerPolicy = EBusHandlerPolicy::Single;
        //////////////////////////////////////////////////////////////////////////

        /**
         * Destroys the instance of the class.
         */
        virtual ~TransformInterface() {}

        //////////////////////////////////////////////////////////////////////////
        // Transform modifiers

        /**
         * Returns the entity's local transform, not including the parent transform.
         * @return A reference to a transform that represents the entity's position
         * relative to its parent entity.
         */
        virtual const Transform& GetLocalTM() = 0;

        /**
         * Sets the entity's local transform and notifies all listeners.
         * @param tm A reference to a transform for positioning the entity
         * relative to its parent entity.
         */
        virtual void SetLocalTM(const Transform& /*tm*/) {}

        /**
         * Returns the entity's world transform, including the parent transform.
         * @return A reference to a transform that represents the entity's position
         * within the world.
         */
        virtual const Transform& GetWorldTM() = 0;

        /**
         * Sets the world transform and notifies all listeners.
         * @param tm A reference to a transform for positioning the entity
         * within the world.
         */
        virtual void SetWorldTM(const Transform& /*tm*/) {}

        /**
         * Retrieves the entity's local and world transforms.
         * @param[out] localTM A reference to a transform that represents the entity's
         * position relative to its parent entity.
         * @param[out] worldTM A reference to a transform that represents the entity's
         * position within the world.
         */
        virtual void GetLocalAndWorld(Transform& /*localTM*/, Transform& /*worldTM*/) {}

        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // Translation modifiers

        /**
         * Sets the entity's world space translation, which represents how
         * to move the entity to a new position within the world.
         * @param newPosition A three-dimensional translation vector.
         */
        virtual void SetWorldTranslation(const AZ::Vector3& /*newPosition*/) {}

        /**
         * Sets the entity's local space translation, which represents how to move the
         * entity to a new position relative to its parent.
         * @param newPosition A three-dimensional translation vector.
         */
        virtual void SetLocalTranslation(const AZ::Vector3& /*newPosition*/) {}

        /**
         * Gets the entity's world space translation.
         * @return A three-dimensional translation vector.
         */
        virtual AZ::Vector3 GetWorldTranslation() { return AZ::Vector3(FLT_MAX); }

        /**
         * Gets the entity's local space translation.
         * @return A three-dimensional translation vector.
         */
        virtual AZ::Vector3 GetLocalTranslation() { return AZ::Vector3(FLT_MAX); }

        /**
         * Moves the entity within world space.
         * @param offset A three-dimensional vector that contains the offset
         * to apply to the entity.
         */
        virtual void MoveEntity(const AZ::Vector3& /*offset*/) {}

        /**
         * Sets the entity's X coordinate in world space.
         * @param x A new value for the entity's X coordinate in world space.
         */
        virtual void SetWorldX(float /*x*/) {}

        /**
         * Sets the entity's Y coordinate in world space.
         * @param y A new value for the entity's Y coordinate in world space.
         */
        virtual void SetWorldY(float /*y*/) {}

        /**
         * Sets the entity's Z coordinate in world space.
         * @param z A new value for the entity's Z coordinate in world space.
         */
        virtual void SetWorldZ(float /*z*/) {}

        /**
         * Gets the entity's X coordinate in world space.
         * @return The entity's X coordinate in world space.
         */
        virtual float GetWorldX() { return FLT_MAX; }

        /**
         * Gets the entity's Y coordinate in world space.
         * @return The entity's Y coordinate in world space.
         */
        virtual float GetWorldY() { return FLT_MAX; }

        /**
         * Gets the entity's Z coordinate in world space.
         * @return The entity's Z coordinate in world space.
         */
        virtual float GetWorldZ() { return FLT_MAX; }

        /**
         * Sets the entity's X coordinate in local space.
         * @param x A new value for the entity's X coordinate in local space.
         */
        virtual void SetLocalX(float /*x*/) {}

        /**
         * Sets the entity's Y coordinate in local space.
         * @param y A new value for the entity's Y coordinate in local space.
         */
        virtual void SetLocalY(float /*y*/) {}

        /**
         * Sets the entity's Z coordinate in local space.
         * @param z A new value for the entity's Z coordinate in local space.
         */
        virtual void SetLocalZ(float /*z*/) {}

        /**
         * Gets the entity's X coordinate in local space.
         * @return The entity's X coordinate in local space.
         */
        virtual float GetLocalX() { return FLT_MAX; }

        /**
         * Gets the entity's Y coordinate in local space.
         * @return The entity's Y coordinate in local space.
         */
        virtual float GetLocalY() { return FLT_MAX; }

        /**
         * Gets the entity's Z coordinate in local space.
         * @return The entity's Z coordinate in local space.
         */
        virtual float GetLocalZ() { return FLT_MAX; }

        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // Rotation modifiers

        /**
         * Get angles in radian for each principle axis around which the world transform is
         * rotated in the order of z-axis and y-axis and then x-axis.
         * @return The Euler angles in radian indicating how much is rotated around each principle axis.
         */
        virtual AZ::Vector3 GetWorldRotation() { return AZ::Vector3(FLT_MAX); }

        /**
         * Get the quaternion representing the world rotation.
         * @return The Rotation quaternion in world space.
         */
        virtual AZ::Quaternion GetWorldRotationQuaternion() { return AZ::Quaternion::CreateZero(); }

        /**
         * Set the local rotation matrix using the composition of rotations around
         * the principle axes in the order of z-axis first and y-axis and then x-axis.
         * @param eulerRadianAngles A Vector3 denoting radian angles of the rotations around each principle axis.
         */
        virtual void SetLocalRotation(const AZ::Vector3& /*eulerRadianAngles*/) {}

        /**
         * Set the local rotation matrix using a quaternion.
         * @param quaternion A quaternion representing the rotation to set.
         */
        virtual void SetLocalRotationQuaternion(const AZ::Quaternion& /*quaternion*/) {}

        /**
         * Rotate around the local x-axis for a radian angle.
         * @param eulerRadianAngle The angle to rotate around the local x-axis.
         */
        virtual void RotateAroundLocalX(float /*eulerAngleRadian*/) {}

        /**
         * Rotate around the local y-axis for a radian angle.
         * @param eulerRadianAngle The angle to rotate around the local y-axis.
         */
        virtual void RotateAroundLocalY(float /*eulerAngleRadian*/) {}

        /**
         * Rotate around the local z-axis for a radian angle.
         * @param eulerRadianAngle The angle to rotate around the local z-axis.
         */
        virtual void RotateAroundLocalZ(float /*eulerAngleRadian*/) {}

        /**
         * Get angles in radian for each principle axis around which the local transform is
         * rotated in the order of z-axis and y-axis and then x-axis.
         * @return A value of type Vector3 indicating how much in radian is rotated around each principle axis.
         */
        virtual AZ::Vector3 GetLocalRotation() { return AZ::Vector3(FLT_MAX); }

        /**
         * Get the quaternion representing the local rotation.
         * @return The rotation quaternion in local space.
         */
        virtual AZ::Quaternion GetLocalRotationQuaternion() { return AZ::Quaternion::CreateZero(); }
        //////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////
        // Scale Modifiers

        /**
         * Set local scale of the transform.
         * @param scale The new scale to set along three local axes.
         */
        virtual void SetLocalScale(const AZ::Vector3& /*scale*/) {}

        /**
         * Set local scale of the transform on x-axis.
         * @param scaleX The new x-axis scale to set.
         */
        virtual void SetLocalScaleX(float /*scaleX*/) {}

        /**
         * Set local scale of the transform on y-axis.
         * @param scaleY The new y-axis scale to set.
         */
        virtual void SetLocalScaleY(float /*scaleY*/) {}

        /**
         * Set local scale of the transform on z-axis.
         * @param scaleZ The new z-axis scale to set.
         */
        virtual void SetLocalScaleZ(float /*scaleZ*/) {}

        /**
         * Get the scale value on each axis in local space
         * @return The scale value of type Vector3 along each axis in local space.
         */
        virtual AZ::Vector3 GetLocalScale() { return AZ::Vector3(FLT_MAX); }

        /**
         * Get the scale value on each axis in world space.
         * Note the transform will be skewed when it is rotated and has a parent transform scaled, in which
         * case the returned world-scale from this function will be inaccurate.
         * @return The scale value of type Vector3 along each axis in world space.
         */
        virtual AZ::Vector3 GetWorldScale() { return AZ::Vector3(FLT_MAX); }
        //////////////////////////////////////////////////////////////////////////

        /**
         * Returns the entity ID of the entity's parent.
         * @return The entity ID of the parent. The entity ID is invalid if the
         * entity does not have a parent with a valid entity ID.
         */
        virtual EntityId GetParentId() { return EntityId(); }

        /**
         * Returns the transform interface of the parent entity.
         * @return A pointer to the transform interface of the parent.
         * Returns a null pointer if no parent is set or the parent
         * entity is not currently activated.
         */
        virtual TransformInterface* GetParent() { return nullptr; }

        /**
         * Sets the entity's parent entity and notifies all listeners.
         * The entity's local transform is moved into the parent entity's space
         * to preserve the entity's world transform.
         * @param id The ID of the entity to set as the parent.
         */
        virtual void SetParent(EntityId /*id*/) {}

        /**
         * Sets the entity's parent entity, moves the transform relative to the
         * parent entity, and notifies all listeners.
         * This function uses the world transform as a local transform and moves the
         * transform relative to the parent entity.
         * @param id The ID of the entity to set as the parent.
         */
        virtual void SetParentRelative(EntityId /*id*/) {}

        /**
         * Returns the entity IDs of the entity's immediate children.
         * @return A vector that contains the entity IDs of the entity's immediate children.
         */
        virtual AZStd::vector<AZ::EntityId> GetChildren() { return AZStd::vector<AZ::EntityId>(); };

        /**
         * Returns the entity IDs of all descendants of the entity. The descendants
         * are the entity's children, the children's children, and so on.
         * The entity IDs are ordered breadth-first.
         * @return A vector that contains the entity IDs of the descendants.
         */
        virtual AZStd::vector<AZ::EntityId> GetAllDescendants() { return AZStd::vector<AZ::EntityId>(); };

        /**
         * Returns the entity ID of the entity and all its descendants. The descendants
         * are the entity's children, the children's children, and so on. The entity IDs
         * are ordered breadth-first and this entity's ID is the first in the list.
         * @return A vector that contains the entity ID of the entity followed by the entity
         * IDs of its descendants.
         */
        virtual AZStd::vector<AZ::EntityId> GetEntityAndAllDescendants() { return AZStd::vector<AZ::EntityId>(); }

        /**
         * Returns whether the transform is static.
         * A static transform is unmovable and does not respond to requests that would move it.
         * @return True if the transform is static, false if the transform is movable.
         */
        virtual bool IsStaticTransform() = 0;
    };


    /**
     * The EBus for requests to position and parent an entity.
     * The events are defined in the AZ::TransformInterface class.
     */
    typedef AZ::EBus<TransformInterface>    TransformBus;


    /**
     * Interface for AZ::TransformNotificationBus, which is the EBus that
     * dispatches transform changes to listeners.
     */
    class TransformNotification
        : public ComponentBus
    {
    public:

        /**
         * Destroys the instance of the class.
         */
        virtual ~TransformNotification() {}


        /**
         * Signals that the local or world transform of the entity changed.
         * @param local A reference to the new local transform of the entity.
         * @param world A reference to the new world transform of the entity.
         */
        virtual void OnTransformChanged(const Transform& /*local*/, const Transform& /*world*/)     { }

        /**
        * Signals that the static flag on the transfrom has changed. This should only be needed during editing.
        * @param isStatic A boolean that indicates whether the transfrom is static or not.
        */
        virtual void OnStaticChanged( bool /*isStatic*/) { }


        /**
         * Signals that the parent of the entity changed.
         * To find if an entity ID is valid, use AZ::EntityId::IsValid().
         * @param oldParent The entity ID of the old parent. The entity ID is invalid if there was no old parent.
         * @param newParent The entity ID of the new parent. The entity ID is invalid if there is no new parent.
         */
        virtual void OnParentChanged(EntityId oldParent, EntityId newParent)    { (void)oldParent; (void)newParent; }

        /**
         * Signals that a child was added to the entity.
         * @param child The entity ID of the added child.
         */
        virtual void OnChildAdded(EntityId child) { (void)child; }

        /**
         * Signals that a child was removed from the entity.
         * @param child The entity ID of the removed child.
         */
        virtual void OnChildRemoved(EntityId child) { (void)child; }
    };

    /**
     * The EBus for transform notification events.
     * The events are defined in the AZ::TransformNotification class.
     */
    typedef AZ::EBus<TransformNotification> TransformNotificationBus;

    /**
     * The type ID of game component AzFramework::TransformComponent.
     */
    static const TypeId TransformComponentTypeId = "{22B10178-39B6-4C12-BB37-77DB45FDD3B6}";

    /**
     * Component configuration for the transform component.
     */
    class TransformConfig
        : public ComponentConfig
    {
    public:
        AZ_RTTI(TransformConfig, "{B3AAB26D-D075-4E2B-9653-9527EE363DF8}", ComponentConfig);
        AZ_CLASS_ALLOCATOR(TransformConfig, SystemAllocator, 0);

        /**
         * Behavior when a parent entity activates.
         * A parent may activate before or after its children have activated.
         */
        enum class ParentActivationTransformMode : u32
        {
            MaintainOriginalRelativeTransform,  ///< Child will snap to originally-configured parent-relative transform when parent is activated.
            MaintainCurrentWorldTransform,      ///< Child will still follow parent, but will maintain its current world transform when parent is activated.
        };

        /**
         * Constructor with all default values.
         * Transform is positioned at (0,0,0) with no rotation and scale of 1.
         */
        TransformConfig() = default;

        /**
         * Constructor which sets a 3D transform.
         * Sets both the local and world transform to the same value.
         * @param transform The entity's position, rotation, and scale in 3D.
         */
        explicit TransformConfig(const Transform& transform)
            : m_worldTransform(transform)
            , m_localTransform(transform)
        {}

        /**
         * World 3D transform.
         * This property is used if no parent is assigned,
         * or if the assigned parent entity cannot be found.
         * This property is ignored if the assigned parent is present.
         */
        Transform m_worldTransform = Transform::Identity();

        /**
         * Local 3D transform, as an offset from the parent entity.
         * This property is used to offset the entity from its parent's world transform.
         * Local transform is ignored if no parent is assigned.
         */
        Transform m_localTransform = Transform::Identity();

        /**
         * ID of parent entity.
         * When the parent entity moves, this transform will follow.
         */
        EntityId m_parentId;

        /**
         * Behavior when the parent entity activates.
         * A parent entity is not guaranteed to activate before its children.
         * If a parent entity activates after its child, this property
         * determines whether the entity maintains its current world transform
         * or snaps to maintain the local transform as an offset from the parent.
         */
        ParentActivationTransformMode m_parentActivationTransformMode = ParentActivationTransformMode::MaintainOriginalRelativeTransform;

        /**
         * Whether the transform is static.
         * A static transform will never move.
         */
        bool m_isStatic = false;
    };

    AZ_TYPE_INFO_SPECIALIZE(TransformConfig::ParentActivationTransformMode, "{03FD8A24-CE8F-4651-A3CC-09F40D36BC2C}");

    /// @cond EXCLUDE_DOCS
    /**
     * Interface for AZ::TransformHierarchyInformationBus, which the transform components
     * of parent entities use to get their children's entity IDs.
     * Only children of a particular entity connect to this bus because they use the
     * parent's entity ID to connect to the bus.
     */
    class TransformHierarchyInformation
        : public AZ::EBusTraits
    {
    public:

        /**
         * Destroys the instance of the class.
         */
        virtual ~TransformHierarchyInformation() = default;

        //////////////////////////////////////////////////////////////////////////
        // EBusTraits overrides
        /**
         * Overrides the default AZ::EBusTraits address policy so that the bus
         * has multiple addresses at which to receive messages. This bus is
         * identified by EntityId. Messages addressed to an ID are received by
         * handlers connected to that ID.
         */
        static const EBusAddressPolicy AddressPolicy = EBusAddressPolicy::ById;

        /**
         * Overrides the default AZ::EBusTraits ID type so that entity IDs are
         * used to access the addresses of the bus.
         */
        typedef EntityId BusIdType;
        //////////////////////////////////////////////////////////////////////////

        /**
         * Gets the entity IDs of the parent entity's children.
         * @param children A vector that contains the entity IDs of the children.
         */
        virtual void GatherChildren(AZStd::vector<AZ::EntityId>& /*children*/) {};
    };

    typedef AZ::EBus<TransformHierarchyInformation> TransformHierarchyInformationBus;

    /// @endcond
}

#endif // AZCORE_TRANSFORM_BUS_H
#pragma once
