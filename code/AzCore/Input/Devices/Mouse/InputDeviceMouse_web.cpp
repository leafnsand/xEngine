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

#include <AzCore/PlatformDef.h>

#if defined(AZ_PLATFORM_EMSCRIPTEN)

#include <AzCore/Input/Devices/Mouse/InputDeviceMouse.h>

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
namespace AZ
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //! Platform specific implementation for web mouse input devices
    class InputDeviceMouseWeb : public InputDeviceMouse::Implementation
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////
        // Allocator
        AZ_CLASS_ALLOCATOR(InputDeviceMouseWeb, AZ::SystemAllocator, 0);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Constructor
        //! \param[in] inputDevice Reference to the input device being implemented
        InputDeviceMouseWeb(InputDeviceMouse& inputDevice);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! Destructor
        ~InputDeviceMouseWeb() override;

    private:
        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceMouse::Implementation::IsConnected
        bool IsConnected() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceMouse::Implementation::SetSystemCursorState
        void SetSystemCursorState(SystemCursorState systemCursorState) override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceMouse::Implementation::GetSystemCursorState
        SystemCursorState GetSystemCursorState() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceMouse::Implementation::SetSystemCursorPositionNormalized
        void SetSystemCursorPositionNormalized(AZ::Vector2 positionNormalized) override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceMouse::Implementation::GetSystemCursorPositionNormalized
        AZ::Vector2 GetSystemCursorPositionNormalized() const override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! \ref AZ::InputDeviceMouse::Implementation::TickInputDevice
        void TickInputDevice() override;

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Mouse event callback
        static EM_BOOL OnMouseEvent(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);

        ////////////////////////////////////////////////////////////////////////////////////////////
        // Mouse wheel event callback
        static EM_BOOL OnWheelEvent(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData);

        ////////////////////////////////////////////////////////////////////////////////////////////
        //! The last absolute mouse position
        AZ::Vector2 m_lastMousePosition;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceMouse::Implementation* InputDeviceMouse::Implementation::Create(InputDeviceMouse& inputDevice)
    {
        return aznew InputDeviceMouseWeb(inputDevice);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceMouseWeb::InputDeviceMouseWeb(InputDeviceMouse& inputDevice)
        : InputDeviceMouse::Implementation(inputDevice)
        , m_lastMousePosition()
    {
        emscripten_set_mousedown_callback("canvas", this, true, InputDeviceMouseWeb::OnMouseEvent);
        emscripten_set_mouseup_callback("canvas", this, true, InputDeviceMouseWeb::OnMouseEvent);
        emscripten_set_mousemove_callback("canvas", this, true, InputDeviceMouseWeb::OnMouseEvent);
        emscripten_set_wheel_callback("canvas", this, true, InputDeviceMouseWeb::OnWheelEvent);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    InputDeviceMouseWeb::~InputDeviceMouseWeb()
    {
        emscripten_set_mousedown_callback("canvas", this, true, nullptr);
        emscripten_set_mouseup_callback("canvas", this, true, nullptr);
        emscripten_set_mousemove_callback("canvas", this, true, nullptr);
        emscripten_set_wheel_callback("canvas", this, true, nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    bool InputDeviceMouseWeb::IsConnected() const
    {
        // Mouse input is always available on web
        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceMouseWeb::SetSystemCursorState(SystemCursorState systemCursorState)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    SystemCursorState InputDeviceMouseWeb::GetSystemCursorState() const
    {
        return SystemCursorState::Unknown;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceMouseWeb::SetSystemCursorPositionNormalized(AZ::Vector2 positionNormalized)
    {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    AZ::Vector2 InputDeviceMouseWeb::GetSystemCursorPositionNormalized() const
    {
        EmscriptenMouseEvent mouseEvent;
        emscripten_get_mouse_status(&mouseEvent);
        
        double windowWidth, windowHeight;
        emscripten_get_element_css_size("canvas", &windowWidth, &windowHeight);

        return AZ::Vector2(mouseEvent.canvasX / static_cast<float>(windowWidth), mouseEvent.canvasY / static_cast<float>(windowHeight));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    void InputDeviceMouseWeb::TickInputDevice()
    {
        // Process the raw event queues once each frame
        ProcessRawEventQueues();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    EM_BOOL InputDeviceMouseWeb::OnMouseEvent(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData)
    {
        InputDeviceMouseWeb* self = (InputDeviceMouseWeb*)userData;

        // Main button pressed, usually the left button or the un-initialized state
        if (mouseEvent->button == 0 && eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Left, true);
        }
        if (mouseEvent->button == 0 && eventType == EMSCRIPTEN_EVENT_MOUSEUP)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Left, false);
        }

        // Auxiliary button pressed, usually the wheel button or the middle button (if present)
        if (mouseEvent->button == 1 && eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Middle, true);
        }
        if (mouseEvent->button == 1 && eventType == EMSCRIPTEN_EVENT_MOUSEUP)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Middle, false);
        }

        // Secondary button pressed, usually the right button
        if (mouseEvent->button == 2 && eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Right, true);
        }
        if (mouseEvent->button == 2 && eventType == EMSCRIPTEN_EVENT_MOUSEUP)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Right, false);
        }

        // Fourth button, typically the Browser Back button
        if (mouseEvent->button == 1 && eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Other1, true);
        }
        if (mouseEvent->button == 1 && eventType == EMSCRIPTEN_EVENT_MOUSEUP)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Other1, false);
        }

        // Fifth button, typically the Browser Forward button
        if (mouseEvent->button == 2 && eventType == EMSCRIPTEN_EVENT_MOUSEDOWN)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Other2, true);
        }
        if (mouseEvent->button == 2 && eventType == EMSCRIPTEN_EVENT_MOUSEUP)
        {
            self->QueueRawButtonEvent(InputDeviceMouse::Button::Other2, false);
        }

        if (eventType == EMSCRIPTEN_EVENT_MOUSEMOVE)
        {
            double windowWidth, windowHeight;
            emscripten_get_element_css_size("canvas", &windowWidth, &windowHeight);

            AZ::Vector2 currentMousePosition(mouseEvent->canvasX / static_cast<float>(windowWidth), mouseEvent->canvasY / static_cast<float>(windowHeight));

            self->QueueRawMovementEvent(InputDeviceMouse::Movement::X, currentMousePosition.GetX() - self->m_lastMousePosition.GetX());
            self->QueueRawMovementEvent(InputDeviceMouse::Movement::Y, currentMousePosition.GetY() - self->m_lastMousePosition.GetY());

            self->m_lastMousePosition = currentMousePosition;
        }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    EM_BOOL InputDeviceMouseWeb::OnWheelEvent(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData)
    {
        InputDeviceMouseWeb* self = (InputDeviceMouseWeb*)userData;

        self->QueueRawMovementEvent(InputDeviceMouse::Movement::Z, static_cast<short>(wheelEvent->deltaY));

        return true;
    }
} // namespace AZ

#endif // defined(AZ_PLATFORM_EMSCRIPTEN)
