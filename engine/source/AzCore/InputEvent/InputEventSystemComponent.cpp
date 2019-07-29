#include "InputEventSystemComponent.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/RTTI/BehaviorContext.h>
#include <AzCore/Input/Devices/Keyboard/InputDeviceKeyboard.h>
#include <AzCore/Input/Devices/Mouse/InputDeviceMouse.h>
#include <AzCore/Input/Devices/Touch/InputDeviceTouch.h>
#include <AzCore/Input/Devices/VirtualKeyboard/InputDeviceVirtualKeyboard.h>
#include <AzCore/Input/Devices/Gamepad/InputDeviceGamepad.h>
#include <AzCore/Input/Devices/Motion/InputDeviceMotion.h>

namespace AZ
{
    class InputEventSystemBusBehaviorHandler : public InputEventSystemNotificationBus::Handler, public AZ::BehaviorEBusHandler
    {
    public:
        AZ_EBUS_BEHAVIOR_BINDER(InputEventSystemBusBehaviorHandler, "{3E1B9C5F-A16C-4C02-8D2D-216A2A66E978}", AZ::SystemAllocator, 
            OnKeyDown, OnKeyHold, OnKeyUp, 
            OnMouseDown, OnMouseMove, OnMouseUp,
            OnTouchDown, OnTouchMove, OnTouchUp);

        void OnKeyDown(AZ::Crc32 value) override
        {
            Call(FN_OnKeyDown, value);
        }

        void OnKeyHold(AZ::Crc32 value) override
        {
            Call(FN_OnKeyHold, value);
        }

        void OnKeyUp(AZ::Crc32 value) override
        {
            Call(FN_OnKeyUp, value);
        }

        void OnMouseDown(AZ::Crc32 index, AZ::Vector3 value) override
        {
            Call(FN_OnMouseDown, index, value);
        }

        void OnMouseMove(AZ::Vector3 value) override
        {
            Call(FN_OnMouseMove, value);
        }

        void OnMouseUp(AZ::Crc32 index, AZ::Vector3 value) override
        {
            Call(FN_OnMouseUp, index, value);
        }

        void OnTouchDown(AZ::Crc32 index, AZ::Vector3 value) override
        {
            Call(FN_OnTouchDown, index, value);
        }

        void OnTouchMove(AZ::Crc32 index, AZ::Vector3 value) override
        {
            Call(FN_OnTouchMove, index ,value);
        }

        void OnTouchUp(AZ::Crc32 index, AZ::Vector3 value) override
        {
            Call(FN_OnTouchUp, index, value);
        }
    };

    void InputEventSystemComponent::Reflect(AZ::ReflectContext* reflection)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(reflection))
        {
            serializeContext->Class<InputEventSystemComponent, AZ::Component>()
                ;
        }

        if (auto behaviorContext = azrtti_cast<AZ::BehaviorContext*>(reflection))
        {
            behaviorContext->EBus<InputEventSystemRequestBus>("InputEventSystemRequestBus");

            behaviorContext->EBus<InputEventSystemNotificationBus>("InputEventSystemNotificationBus")
                ->Handler<InputEventSystemBusBehaviorHandler>();
        }
    }

    InputEventSystemComponent::InputEventSystemComponent()
    {

    }

    void InputEventSystemComponent::Init()
    {
    }

    void InputEventSystemComponent::Activate()
    {
        InputEventSystemRequestBus::Handler::BusConnect();
        AZ::InputChannelEventListener::BusConnect();
    }

    void InputEventSystemComponent::Deactivate()
    {
        AZ::InputChannelEventListener::BusDisconnect();
        InputEventSystemRequestBus::Handler::BusDisconnect();
    }

    //--------------------------------------------------------------------------------------------------------
    bool InputEventSystemComponent::OnInputChannelEventFiltered(const AZ::InputChannel& inputChannel)
    {
        if(AZ::InputDeviceKeyboard::IsKeyboardDevice(inputChannel.GetInputDevice().GetInputDeviceId()))
        {
            const AZ::InputChannel::Snapshot inputSnapshot(inputChannel);
            if (inputSnapshot.m_state == AZ::InputChannel::State::Began)
            {
                EBUS_EVENT(InputEventSystemNotificationBus, OnKeyDown, inputChannel.GetInputChannelId().GetNameCrc32());
            }
            else  if (inputSnapshot.m_state == AZ::InputChannel::State::Updated)
            {
                EBUS_EVENT(InputEventSystemNotificationBus, OnKeyHold, inputChannel.GetInputChannelId().GetNameCrc32());
            }
            else  if (inputSnapshot.m_state == AZ::InputChannel::State::Ended)
            {
                EBUS_EVENT(InputEventSystemNotificationBus, OnKeyUp, inputChannel.GetInputChannelId().GetNameCrc32());
            }
        }
        else if(AZ::InputDeviceMouse::IsMouseDevice(inputChannel.GetInputDevice().GetInputDeviceId()))
        {
            AZ::Vector2 viewportPos(0.0f, 0.0f);
            const AZ::InputChannel::PositionData2D* positionData2D = inputChannel.GetCustomData<AZ::InputChannel::PositionData2D>();
            if (positionData2D)
            {
                viewportPos.SetX(positionData2D->m_normalizedPosition.GetX());
                viewportPos.SetY(positionData2D->m_normalizedPosition.GetY());

                const AZ::InputChannel::Snapshot inputSnapshot(inputChannel);
                if (inputSnapshot.m_state == AZ::InputChannel::State::Began)
                {
                    if (inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::SystemCursorPosition
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::X
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Y
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Z
                        && positionData2D->m_normalizedPosition.GetX() >= 0
                        && positionData2D->m_normalizedPosition.GetX() <= 1
                        && positionData2D->m_normalizedPosition.GetY() >= 0
                        && positionData2D->m_normalizedPosition.GetY() <= 1)
                    {
                        EBUS_EVENT(InputEventSystemNotificationBus, OnMouseDown, inputChannel.GetInputChannelId().GetNameCrc32(), AZ::Vector3(viewportPos.GetX(), viewportPos.GetY(), 0));
                    }
                }

                if(inputChannel.GetInputChannelId() == AZ::InputDeviceMouse::SystemCursorPosition
                    && positionData2D->m_normalizedPosition.GetX() >= 0
                    && positionData2D->m_normalizedPosition.GetX() <= 1
                    && positionData2D->m_normalizedPosition.GetY() >= 0
                    && positionData2D->m_normalizedPosition.GetY() <= 1)
                {
                    EBUS_EVENT(InputEventSystemNotificationBus, OnMouseMove, AZ::Vector3(viewportPos.GetX(), viewportPos.GetY(), 0));
                }

                if (inputSnapshot.m_state == AZ::InputChannel::State::Ended)
                {
                    if (inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::SystemCursorPosition
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::X
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Y
                        && inputChannel.GetInputChannelId() != AZ::InputDeviceMouse::Movement::Z)
                    {
                        EBUS_EVENT(InputEventSystemNotificationBus, OnMouseUp, inputChannel.GetInputChannelId().GetNameCrc32(), AZ::Vector3(viewportPos.GetX(), viewportPos.GetY(), 0));
                    }
                }
            }
        }
        else if (AZ::InputDeviceTouch::IsTouchDevice(inputChannel.GetInputDevice().GetInputDeviceId()))
        {
            AZ::Vector2 viewportPos(0.0f, 0.0f);
            const AZ::InputChannel::PositionData2D* positionData2D = inputChannel.GetCustomData<AZ::InputChannel::PositionData2D>();
            if (positionData2D)
            {
                viewportPos.SetX(positionData2D->m_normalizedPosition.GetX());
                viewportPos.SetY(positionData2D->m_normalizedPosition.GetY());

                const AZ::InputChannel::Snapshot inputSnapshot(inputChannel);
                if (inputSnapshot.m_state == AZ::InputChannel::State::Began)
                {
                    EBUS_EVENT(InputEventSystemNotificationBus, OnTouchDown, inputChannel.GetInputChannelId().GetNameCrc32(), AZ::Vector3(viewportPos.GetX(), viewportPos.GetY(), 0));
                }

                if (inputSnapshot.m_state == AZ::InputChannel::State::Updated)
                {
                    EBUS_EVENT(InputEventSystemNotificationBus, OnTouchMove, inputChannel.GetInputChannelId().GetNameCrc32(), AZ::Vector3(viewportPos.GetX(), viewportPos.GetY(), 0));
                }

                if (inputSnapshot.m_state == AZ::InputChannel::State::Ended)
                {
                    EBUS_EVENT(InputEventSystemNotificationBus, OnTouchUp, inputChannel.GetInputChannelId().GetNameCrc32(), AZ::Vector3(viewportPos.GetX(), viewportPos.GetY(), 0));
                }
            }
        }
        else if (AZ::InputDeviceVirtualKeyboard::IsVirtualKeyboardDevice(inputChannel.GetInputDevice().GetInputDeviceId())){}
        else if (AZ::InputDeviceGamepad::IsGamepadDevice(inputChannel.GetInputDevice().GetInputDeviceId())){}
        else if (AZ::InputDeviceMotion::IsMotionDevice(inputChannel.GetInputDevice().GetInputDeviceId())){}

        return false;
    }
}
