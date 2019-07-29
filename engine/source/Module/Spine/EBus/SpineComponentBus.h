#pragma once

#include <AzCore/Component/ComponentBus.h>

namespace Module
{
    enum class SpineAnimationEventType {
        Start,
        Complete,
        End,
    };

    class SpineRequest : public AZ::ComponentBus
    {
    public:
        static const bool EnableEventQueue = true;

        virtual bool IsValid() const = 0;

        virtual void SetSkin(const AZStd::string& skinName) = 0;

        virtual void SetMix(const AZStd::string& fromAnim, const AZStd::string& toAnim, float duration) = 0;

        virtual void ClearTracks() = 0;

        virtual void ClearTrack(int trackIndex) = 0;

        virtual void SetAnimation(int trackIndex, const AZStd::string& animationName, bool loop) = 0;

        virtual void AddAnimation(int trackIndex, const AZStd::string& animationName, bool loop, float delay) = 0;

        virtual void SetEmptyAnimation(int trackIndex, float mixDuration) = 0;

        virtual void AddEmptyAnimation(int trackIndex, float mixDuration, float delay) = 0;

        virtual void SetEmptyAnimations(float mixDuration) = 0;
    };

    using SpineRequestBus = AZ::EBus<SpineRequest>;

    class SpineNotification : public AZ::ComponentBus
    {
    public:
        virtual void OnAnimationEvent(const AZStd::string& name, SpineAnimationEventType type) {}
    };

    using SpineNotificationBus = AZ::EBus<SpineNotification>;
}